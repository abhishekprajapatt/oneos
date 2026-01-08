#include "watchdog_deadlock_detector.h"
#include <stdlib.h>
#include <string.h>

static WatchdogDeadlockDetector *global_watchdog_detector = NULL;

WatchdogDeadlockDetector *watchdog_init(void)
{
    WatchdogDeadlockDetector *detector = (WatchdogDeadlockDetector *)malloc(sizeof(WatchdogDeadlockDetector));
    if (!detector)
        return NULL;

    memset(detector, 0, sizeof(WatchdogDeadlockDetector));
    detector->detector_id = 1;
    detector->watched_count = 0;
    detector->lock_dep_count = 0;
    detector->deadlock_cycle_count = 0;
    detector->total_timeouts_detected = 0;
    detector->total_deadlocks_detected = 0;
    detector->deadlock_resolutions = 0;

    global_watchdog_detector = detector;
    return detector;
}

int watchdog_destroy(WatchdogDeadlockDetector *detector)
{
    if (!detector)
        return -1;

    free(detector);
    global_watchdog_detector = NULL;
    return 0;
}

int watchdog_register_process(WatchdogDeadlockDetector *detector, uint32_t process_id,
                              uint32_t timeout_seconds)
{
    if (!detector || detector->watched_count >= MAX_WATCHED_PROCESSES || process_id == 0)
        return -1;

    WatchdogEntry *entry = &detector->watched_processes[detector->watched_count];
    memset(entry, 0, sizeof(WatchdogEntry));

    entry->process_id = process_id;
    entry->thread_id = 0;
    entry->state = WATCHDOG_STATE_HEALTHY;
    entry->last_heartbeat_time = 0;
    entry->timeout_seconds = (timeout_seconds > 0) ? timeout_seconds : 30;
    entry->watchdog_count = 0;
    entry->consecutive_timeouts = 0;
    entry->is_monitored = 1;

    detector->watched_count++;
    return 0;
}

int watchdog_unregister_process(WatchdogDeadlockDetector *detector, uint32_t process_id)
{
    if (!detector || process_id == 0)
        return -1;

    for (uint32_t i = 0; i < detector->watched_count; i++)
    {
        if (detector->watched_processes[i].process_id == process_id)
        {
            memmove(&detector->watched_processes[i], &detector->watched_processes[i + 1],
                    sizeof(WatchdogEntry) * (detector->watched_count - i - 1));
            detector->watched_count--;
            return 0;
        }
    }

    return -1;
}

int watchdog_heartbeat(WatchdogDeadlockDetector *detector, uint32_t process_id)
{
    if (!detector || process_id == 0)
        return -1;

    for (uint32_t i = 0; i < detector->watched_count; i++)
    {
        if (detector->watched_processes[i].process_id == process_id)
        {
            WatchdogEntry *entry = &detector->watched_processes[i];
            entry->last_heartbeat_time = 0;
            entry->watchdog_count++;
            entry->consecutive_timeouts = 0;
            entry->state = WATCHDOG_STATE_HEALTHY;
            return 0;
        }
    }

    return -1;
}

int watchdog_check_timeout(WatchdogDeadlockDetector *detector, uint32_t process_id)
{
    if (!detector || process_id == 0)
        return -1;

    for (uint32_t i = 0; i < detector->watched_count; i++)
    {
        if (detector->watched_processes[i].process_id == process_id)
        {
            WatchdogEntry *entry = &detector->watched_processes[i];

            uint64_t elapsed = 0;
            if (elapsed > (uint64_t)entry->timeout_seconds * 1000000)
            {
                entry->state = WATCHDOG_STATE_TIMEOUT;
                entry->consecutive_timeouts++;
                return 1;
            }

            return 0;
        }
    }

    return -1;
}

int watchdog_detect_all_timeouts(WatchdogDeadlockDetector *detector)
{
    if (!detector)
        return 0;

    uint32_t timeout_count = 0;

    for (uint32_t i = 0; i < detector->watched_count; i++)
    {
        if (watchdog_check_timeout(detector, detector->watched_processes[i].process_id) > 0)
        {
            timeout_count++;
            detector->total_timeouts_detected++;
        }
    }

    return timeout_count;
}

int watchdog_handle_timeout(WatchdogDeadlockDetector *detector, uint32_t process_id)
{
    if (!detector || process_id == 0)
        return -1;

    for (uint32_t i = 0; i < detector->watched_count; i++)
    {
        if (detector->watched_processes[i].process_id == process_id)
        {
            WatchdogEntry *entry = &detector->watched_processes[i];

            if (entry->consecutive_timeouts >= 3)
            {
                return deadlock_kill_process(detector, process_id);
            }

            return 0;
        }
    }

    return -1;
}

int lock_dependency_add(WatchdogDeadlockDetector *detector, uint32_t lock_id,
                        uint32_t holder_id, uint32_t waiter_id)
{
    if (!detector || detector->lock_dep_count >= MAX_LOCK_DEPENDENCIES)
        return -1;

    LockDependency *dep = &detector->lock_deps[detector->lock_dep_count];
    memset(dep, 0, sizeof(LockDependency));

    dep->lock_id = lock_id;
    dep->holder_process_id = holder_id;
    dep->waiter_process_id = waiter_id;
    dep->lock_acquired_time = 0;
    dep->wait_start_time = 0;
    dep->wait_duration_us = 0;

    detector->lock_dep_count++;
    return 0;
}

int lock_dependency_remove(WatchdogDeadlockDetector *detector, uint32_t lock_id)
{
    if (!detector || lock_id == 0)
        return -1;

    for (uint32_t i = 0; i < detector->lock_dep_count; i++)
    {
        if (detector->lock_deps[i].lock_id == lock_id)
        {
            memmove(&detector->lock_deps[i], &detector->lock_deps[i + 1],
                    sizeof(LockDependency) * (detector->lock_dep_count - i - 1));
            detector->lock_dep_count--;
            return 0;
        }
    }

    return -1;
}

int deadlock_detect_cycles(WatchdogDeadlockDetector *detector)
{
    if (!detector)
        return 0;

    uint32_t cycle_count = 0;

    for (uint32_t i = 0; i < detector->lock_dep_count && cycle_count < MAX_DEADLOCK_CYCLES; i++)
    {
        LockDependency *dep = &detector->lock_deps[i];

        for (uint32_t j = 0; j < detector->lock_dep_count; j++)
        {
            LockDependency *other = &detector->lock_deps[j];

            if (other->holder_process_id == dep->waiter_process_id &&
                other->waiter_process_id == dep->holder_process_id)
            {

                DeadlockCycle *cycle = &detector->deadlock_cycles[cycle_count];
                memset(cycle, 0, sizeof(DeadlockCycle));

                cycle->cycle_id = cycle_count + 1;
                cycle->process_ids[0] = dep->holder_process_id;
                cycle->process_ids[1] = dep->waiter_process_id;
                cycle->process_count = 2;
                cycle->detected_time = 0;
                cycle->is_resolved = 0;

                cycle_count++;
                detector->total_deadlocks_detected++;
                break;
            }
        }
    }

    detector->deadlock_cycle_count = cycle_count;
    return cycle_count;
}

int deadlock_resolve_cycle(WatchdogDeadlockDetector *detector, uint32_t cycle_id)
{
    if (!detector || cycle_id == 0)
        return -1;

    for (uint32_t i = 0; i < detector->deadlock_cycle_count; i++)
    {
        if (detector->deadlock_cycles[i].cycle_id == cycle_id)
        {
            DeadlockCycle *cycle = &detector->deadlock_cycles[i];

            if (cycle->process_count > 0)
            {
                deadlock_kill_process(detector, cycle->process_ids[0]);
            }

            cycle->is_resolved = 1;
            detector->deadlock_resolutions++;
            return 0;
        }
    }

    return -1;
}

int deadlock_kill_process(WatchdogDeadlockDetector *detector, uint32_t process_id)
{
    if (!detector || process_id == 0)
        return -1;

    return 0;
}

int deadlock_get_cycle_info(WatchdogDeadlockDetector *detector, uint32_t cycle_id,
                            DeadlockCycle *cycle)
{
    if (!detector || cycle_id == 0 || !cycle)
        return -1;

    for (uint32_t i = 0; i < detector->deadlock_cycle_count; i++)
    {
        if (detector->deadlock_cycles[i].cycle_id == cycle_id)
        {
            memcpy(cycle, &detector->deadlock_cycles[i], sizeof(DeadlockCycle));
            return 0;
        }
    }

    return -1;
}

int watchdog_get_metrics(WatchdogDeadlockDetector *detector, WatchdogMetrics *metrics)
{
    if (!detector || !metrics)
        return -1;

    metrics->detector_id = detector->detector_id;
    metrics->monitored_processes = detector->watched_count;
    metrics->total_timeouts = detector->total_timeouts_detected;
    metrics->total_deadlocks = detector->total_deadlocks_detected;
    metrics->deadlock_resolutions = detector->deadlock_resolutions;

    if (detector->watched_count > 0)
    {
        metrics->timeout_rate = (float)detector->total_timeouts_detected / detector->watched_count;
    }
    else
    {
        metrics->timeout_rate = 0.0f;
    }

    metrics->deadlock_rate = (float)detector->total_deadlocks_detected / 10000.0f;

    return 0;
}
