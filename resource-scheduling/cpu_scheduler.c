#include "cpu_scheduler.h"
#include <stdlib.h>
#include <string.h>

static CPUScheduler *global_cpu_scheduler = NULL;

CPUScheduler *cpu_scheduler_init(SchedulingPolicy policy, uint32_t cpu_count)
{
    CPUScheduler *scheduler = (CPUScheduler *)malloc(sizeof(CPUScheduler));
    if (!scheduler)
        return NULL;

    memset(scheduler, 0, sizeof(CPUScheduler));
    scheduler->scheduler_id = 1;
    scheduler->policy = policy;
    scheduler->runqueue_count = 0;
    scheduler->thread_count = 0;
    scheduler->domain_count = 0;
    scheduler->total_threads_scheduled = 0;
    scheduler->total_migrations = 0;
    scheduler->load_balancing_operations = 0;

    for (uint32_t i = 0; i < cpu_count && i < MAX_RUNQUEUES; i++)
    {
        cpu_scheduler_add_runqueue(scheduler, i);
    }

    global_cpu_scheduler = scheduler;
    return scheduler;
}

int cpu_scheduler_destroy(CPUScheduler *scheduler)
{
    if (!scheduler)
        return -1;

    free(scheduler);
    global_cpu_scheduler = NULL;
    return 0;
}

int cpu_scheduler_add_runqueue(CPUScheduler *scheduler, uint32_t cpu_id)
{
    if (!scheduler || scheduler->runqueue_count >= MAX_RUNQUEUES)
        return -1;

    RunQueue *runqueue = &scheduler->runqueues[scheduler->runqueue_count];
    memset(runqueue, 0, sizeof(RunQueue));

    runqueue->cpu_id = cpu_id;
    runqueue->load = 0;
    runqueue->queue_length = 0;
    runqueue->total_context_switches = 0;
    runqueue->idle_time_ns = 0;
    runqueue->active_time_ns = 0;

    scheduler->runqueue_count++;
    return 0;
}

int cpu_scheduler_remove_runqueue(CPUScheduler *scheduler, uint32_t cpu_id)
{
    if (!scheduler || cpu_id >= scheduler->runqueue_count)
        return -1;

    memmove(&scheduler->runqueues[cpu_id], &scheduler->runqueues[cpu_id + 1],
            sizeof(RunQueue) * (scheduler->runqueue_count - cpu_id - 1));
    scheduler->runqueue_count--;

    return 0;
}

uint32_t cpu_scheduler_enqueue_thread(CPUScheduler *scheduler, uint32_t thread_id,
                                      uint32_t process_id, int priority, uint32_t cpu_affinity)
{
    if (!scheduler || scheduler->thread_count >= MAX_SCHEDULABLE_THREADS)
        return 0;

    ScheduleEntry *entry = &scheduler->threads[scheduler->thread_count];
    memset(entry, 0, sizeof(ScheduleEntry));

    entry->thread_id = thread_id;
    entry->process_id = process_id;
    entry->priority = priority;
    entry->state = THREAD_STATE_RUNNABLE;
    entry->cpu_time_ns = 0;
    entry->vruntime_ns = 0;
    entry->time_slice_ms = 10;
    entry->cpu_affinity = cpu_affinity;
    entry->context_switches = 0;
    entry->total_schedules = 0;

    scheduler->thread_count++;
    scheduler->total_threads_scheduled++;

    uint32_t target_cpu = cpu_affinity % scheduler->runqueue_count;
    if (target_cpu < scheduler->runqueue_count)
    {
        RunQueue *runqueue = &scheduler->runqueues[target_cpu];
        if (runqueue->queue_length < MAX_SCHEDULABLE_THREADS)
        {
            runqueue->queue[runqueue->queue_length] = *entry;
            runqueue->queue_length++;
            runqueue->load++;
        }
    }

    return thread_id;
}

int cpu_scheduler_dequeue_thread(CPUScheduler *scheduler, uint32_t cpu_id, uint32_t thread_id)
{
    if (!scheduler || cpu_id >= scheduler->runqueue_count || thread_id == 0)
        return -1;

    RunQueue *runqueue = &scheduler->runqueues[cpu_id];

    for (uint32_t i = 0; i < runqueue->queue_length; i++)
    {
        if (runqueue->queue[i].thread_id == thread_id)
        {
            memmove(&runqueue->queue[i], &runqueue->queue[i + 1],
                    sizeof(ScheduleEntry) * (runqueue->queue_length - i - 1));
            runqueue->queue_length--;
            runqueue->load--;
            return 0;
        }
    }

    return -1;
}

uint32_t cpu_scheduler_select_next_thread(CPUScheduler *scheduler, uint32_t cpu_id)
{
    if (!scheduler || cpu_id >= scheduler->runqueue_count)
        return 0;

    RunQueue *runqueue = &scheduler->runqueues[cpu_id];

    if (runqueue->queue_length == 0)
        return 0;

    switch (scheduler->policy)
    {
    case SCHED_POLICY_FIFO:
    case SCHED_POLICY_ROUND_ROBIN:
        return runqueue->queue[0].thread_id;

    case SCHED_POLICY_CFS:
    {
        uint32_t min_vruntime_idx = 0;
        uint64_t min_vruntime = runqueue->queue[0].vruntime_ns;

        for (uint32_t i = 1; i < runqueue->queue_length; i++)
        {
            if (runqueue->queue[i].vruntime_ns < min_vruntime)
            {
                min_vruntime = runqueue->queue[i].vruntime_ns;
                min_vruntime_idx = i;
            }
        }

        return runqueue->queue[min_vruntime_idx].thread_id;
    }

    case SCHED_POLICY_RT:
    {
        int max_priority = runqueue->queue[0].priority;
        uint32_t max_priority_idx = 0;

        for (uint32_t i = 1; i < runqueue->queue_length; i++)
        {
            if (runqueue->queue[i].priority > max_priority)
            {
                max_priority = runqueue->queue[i].priority;
                max_priority_idx = i;
            }
        }

        return runqueue->queue[max_priority_idx].thread_id;
    }

    default:
        return runqueue->queue[0].thread_id;
    }
}

int cpu_scheduler_run_thread(CPUScheduler *scheduler, uint32_t cpu_id, uint32_t thread_id)
{
    if (!scheduler || cpu_id >= scheduler->runqueue_count || thread_id == 0)
        return -1;

    RunQueue *runqueue = &scheduler->runqueues[cpu_id];

    for (uint32_t i = 0; i < runqueue->queue_length; i++)
    {
        if (runqueue->queue[i].thread_id == thread_id)
        {
            runqueue->queue[i].state = THREAD_STATE_RUNNING;
            runqueue->queue[i].total_schedules++;
            runqueue->total_context_switches++;
            runqueue->active_time_ns += 1000000;
            return 0;
        }
    }

    return -1;
}

int cpu_scheduler_update_vruntime(CPUScheduler *scheduler, uint32_t thread_id, uint64_t runtime_ns)
{
    if (!scheduler || thread_id == 0)
        return -1;

    for (uint32_t i = 0; i < scheduler->thread_count; i++)
    {
        if (scheduler->threads[i].thread_id == thread_id)
        {
            scheduler->threads[i].vruntime_ns += runtime_ns;
            scheduler->threads[i].cpu_time_ns += runtime_ns;
            return 0;
        }
    }

    return -1;
}

int cpu_scheduler_check_need_resched(CPUScheduler *scheduler, uint32_t cpu_id)
{
    if (!scheduler || cpu_id >= scheduler->runqueue_count)
        return -1;

    RunQueue *runqueue = &scheduler->runqueues[cpu_id];
    if (runqueue->queue_length > 1)
    {
        return 1;
    }

    return 0;
}

int cpu_scheduler_balance_load(CPUScheduler *scheduler, uint32_t domain_id)
{
    if (!scheduler || domain_id >= scheduler->domain_count)
        return -1;

    SchedulerDomain *domain = &scheduler->domains[domain_id];
    scheduler->load_balancing_operations++;

    return 0;
}

int cpu_scheduler_migrate_thread(CPUScheduler *scheduler, uint32_t thread_id, uint32_t target_cpu)
{
    if (!scheduler || thread_id == 0 || target_cpu >= scheduler->runqueue_count)
        return -1;

    for (uint32_t i = 0; i < scheduler->runqueue_count; i++)
    {
        RunQueue *runqueue = &scheduler->runqueues[i];

        for (uint32_t j = 0; j < runqueue->queue_length; j++)
        {
            if (runqueue->queue[j].thread_id == thread_id)
            {
                ScheduleEntry thread = runqueue->queue[j];

                cpu_scheduler_dequeue_thread(scheduler, i, thread_id);

                RunQueue *target_runqueue = &scheduler->runqueues[target_cpu];
                if (target_runqueue->queue_length < MAX_SCHEDULABLE_THREADS)
                {
                    target_runqueue->queue[target_runqueue->queue_length] = thread;
                    target_runqueue->queue_length++;
                    target_runqueue->load++;

                    scheduler->total_migrations++;
                    return 0;
                }

                return -1;
            }
        }
    }

    return -1;
}

int cpu_scheduler_find_load_balanced_cpu(CPUScheduler *scheduler, uint32_t thread_id)
{
    if (!scheduler)
        return -1;

    uint32_t min_load = UINT32_MAX;
    uint32_t best_cpu = 0;

    for (uint32_t i = 0; i < scheduler->runqueue_count; i++)
    {
        if (scheduler->runqueues[i].load < min_load)
        {
            min_load = scheduler->runqueues[i].load;
            best_cpu = i;
        }
    }

    return best_cpu;
}

int cpu_scheduler_update_runqueue_loads(CPUScheduler *scheduler)
{
    if (!scheduler)
        return -1;

    for (uint32_t i = 0; i < scheduler->runqueue_count; i++)
    {
        scheduler->runqueues[i].load = scheduler->runqueues[i].queue_length;
    }

    return 0;
}

int cpu_scheduler_wake_up_thread(CPUScheduler *scheduler, uint32_t thread_id)
{
    if (!scheduler || thread_id == 0)
        return -1;

    for (uint32_t i = 0; i < scheduler->thread_count; i++)
    {
        if (scheduler->threads[i].thread_id == thread_id)
        {
            if (scheduler->threads[i].state == THREAD_STATE_SLEEPING ||
                scheduler->threads[i].state == THREAD_STATE_BLOCKED)
            {
                scheduler->threads[i].state = THREAD_STATE_RUNNABLE;
                return 0;
            }
        }
    }

    return -1;
}

int cpu_scheduler_sleep_thread(CPUScheduler *scheduler, uint32_t thread_id)
{
    if (!scheduler || thread_id == 0)
        return -1;

    for (uint32_t i = 0; i < scheduler->thread_count; i++)
    {
        if (scheduler->threads[i].thread_id == thread_id)
        {
            if (scheduler->threads[i].state == THREAD_STATE_RUNNABLE)
            {
                scheduler->threads[i].state = THREAD_STATE_SLEEPING;
                return 0;
            }
        }
    }

    return -1;
}

int cpu_scheduler_get_metrics(CPUScheduler *scheduler, CPUSchedulerMetrics *metrics)
{
    if (!scheduler || !metrics)
        return -1;

    uint32_t runnable = 0, running = 0, sleeping = 0;
    uint32_t total_load = 0;

    for (uint32_t i = 0; i < scheduler->thread_count; i++)
    {
        if (scheduler->threads[i].state == THREAD_STATE_RUNNABLE)
            runnable++;
        else if (scheduler->threads[i].state == THREAD_STATE_RUNNING)
            running++;
        else if (scheduler->threads[i].state == THREAD_STATE_SLEEPING)
            sleeping++;
    }

    for (uint32_t i = 0; i < scheduler->runqueue_count; i++)
    {
        total_load += scheduler->runqueues[i].load;
    }

    metrics->scheduler_id = scheduler->scheduler_id;
    metrics->total_threads = scheduler->thread_count;
    metrics->runnable_threads = runnable;
    metrics->running_threads = running;
    metrics->sleeping_threads = sleeping;
    metrics->total_migrations = scheduler->total_migrations;

    if (scheduler->runqueue_count > 0)
    {
        metrics->average_load = (float)total_load / scheduler->runqueue_count;
    }
    else
    {
        metrics->average_load = 0.0f;
    }

    if (scheduler->total_threads_scheduled > 0)
    {
        metrics->scheduling_efficiency = 100.0f - ((float)scheduler->load_balancing_operations /
                                                   scheduler->total_threads_scheduled * 100.0f);
    }
    else
    {
        metrics->scheduling_efficiency = 0.0f;
    }

    return 0;
}
