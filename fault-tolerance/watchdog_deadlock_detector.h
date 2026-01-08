#ifndef WATCHDOG_DEADLOCK_DETECTOR_H
#define WATCHDOG_DEADLOCK_DETECTOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_WATCHED_PROCESSES 512
#define MAX_LOCK_DEPENDENCIES 4096
#define MAX_DEADLOCK_CYCLES 256

typedef enum
{
    WATCHDOG_STATE_HEALTHY,
    WATCHDOG_STATE_DELAYED,
    WATCHDOG_STATE_STALLED,
    WATCHDOG_STATE_TIMEOUT
} WatchdogState;

typedef struct
{
    uint32_t process_id;
    uint32_t thread_id;

    WatchdogState state;
    uint64_t last_heartbeat_time;
    uint32_t timeout_seconds;

    uint64_t watchdog_count;
    uint32_t consecutive_timeouts;

    uint8_t is_monitored;
} WatchdogEntry;

typedef struct
{
    uint32_t lock_id;
    uint32_t holder_process_id;
    uint32_t waiter_process_id;

    uint64_t lock_acquired_time;
    uint64_t wait_start_time;
    uint32_t wait_duration_us;
} LockDependency;

typedef struct
{
    uint32_t cycle_id;
    uint32_t process_ids[16];
    uint32_t process_count;

    uint64_t detected_time;
    uint8_t is_resolved;
} DeadlockCycle;

typedef struct
{
    uint32_t detector_id;
    WatchdogEntry watched_processes[MAX_WATCHED_PROCESSES];
    uint32_t watched_count;

    LockDependency lock_deps[MAX_LOCK_DEPENDENCIES];
    uint32_t lock_dep_count;

    DeadlockCycle deadlock_cycles[MAX_DEADLOCK_CYCLES];
    uint32_t deadlock_cycle_count;

    uint64_t total_timeouts_detected;
    uint64_t total_deadlocks_detected;
    uint32_t deadlock_resolutions;
} WatchdogDeadlockDetector;

WatchdogDeadlockDetector *watchdog_init(void);
int watchdog_destroy(WatchdogDeadlockDetector *detector);

int watchdog_register_process(WatchdogDeadlockDetector *detector, uint32_t process_id,
                              uint32_t timeout_seconds);
int watchdog_unregister_process(WatchdogDeadlockDetector *detector, uint32_t process_id);

int watchdog_heartbeat(WatchdogDeadlockDetector *detector, uint32_t process_id);
int watchdog_check_timeout(WatchdogDeadlockDetector *detector, uint32_t process_id);

int watchdog_detect_all_timeouts(WatchdogDeadlockDetector *detector);
int watchdog_handle_timeout(WatchdogDeadlockDetector *detector, uint32_t process_id);

int lock_dependency_add(WatchdogDeadlockDetector *detector, uint32_t lock_id,
                        uint32_t holder_id, uint32_t waiter_id);
int lock_dependency_remove(WatchdogDeadlockDetector *detector, uint32_t lock_id);

int deadlock_detect_cycles(WatchdogDeadlockDetector *detector);
int deadlock_resolve_cycle(WatchdogDeadlockDetector *detector, uint32_t cycle_id);
int deadlock_kill_process(WatchdogDeadlockDetector *detector, uint32_t process_id);

int deadlock_get_cycle_info(WatchdogDeadlockDetector *detector, uint32_t cycle_id,
                            DeadlockCycle *cycle);

typedef struct
{
    uint32_t detector_id;
    uint32_t monitored_processes;
    uint64_t total_timeouts;
    uint64_t total_deadlocks;
    uint32_t deadlock_resolutions;
    float timeout_rate;
    float deadlock_rate;
} WatchdogMetrics;

int watchdog_get_metrics(WatchdogDeadlockDetector *detector, WatchdogMetrics *metrics);

#endif
