#include "task_scheduler.h"
#include <stdlib.h>
#include <string.h>

static TaskScheduler *global_scheduler = NULL;
static uint32_t next_task_id = 1;

TaskScheduler *scheduler_init(SchedulingPolicy policy, uint32_t core_count)
{
    TaskScheduler *sched = (TaskScheduler *)malloc(sizeof(TaskScheduler));
    if (!sched)
        return NULL;

    memset(sched, 0, sizeof(TaskScheduler));
    sched->scheduler_id = 1;
    sched->policy = policy;
    sched->core_count = (core_count > MAX_CPU_CORES) ? MAX_CPU_CORES : core_count;
    sched->total_tasks = 0;
    sched->total_scheduling_decisions = 0;
    sched->total_preemptions = 0;
    sched->average_response_time = 0.0f;
    sched->average_turnaround_time = 0.0f;

    for (uint32_t i = 0; i < sched->core_count; i++)
    {
        sched->runqueues[i].runqueue_id = i;
        sched->runqueues[i].cpu_core = i;
        sched->runqueues[i].task_count = 0;
        sched->runqueues[i].current_task_index = 0;
        sched->runqueues[i].time_slice_ms = 10;
        sched->runqueues[i].total_exec_time = 0;
        sched->runqueues[i].context_switch_count = 0;
        sched->runqueues[i].average_utilization = 0.0f;
    }

    global_scheduler = sched;
    return sched;
}

int scheduler_destroy(TaskScheduler *sched)
{
    if (!sched)
        return -1;

    free(sched);
    global_scheduler = NULL;
    return 0;
}

Task *scheduler_create_task(TaskScheduler *sched, const char *name, uint32_t priority)
{
    if (!sched || sched->total_tasks >= MAX_TASKS || !name)
        return NULL;

    Task *task = &sched->tasks[sched->total_tasks];
    memset(task, 0, sizeof(Task));

    task->task_id = next_task_id++;
    strncpy(task->task_name, name, 63);
    task->priority = priority;
    task->state = TASK_STATE_CREATED;
    task->cpu_affinity = 0xFFFFFFFF; // All cores by default
    task->creation_time = 0;
    task->start_time = 0;
    task->end_time = 0;
    task->cpu_time_ms = 0;
    task->wait_time_ms = 0;
    task->preemption_count = 0;
    task->context_switches = 0;
    task->cpu_utilization = 0.0f;

    sched->total_tasks++;
    return task;
}

int scheduler_destroy_task(TaskScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id)
        {
            sched->tasks[i].state = TASK_STATE_TERMINATED;
            return 0;
        }
    }

    return -1;
}

int scheduler_set_task_priority(TaskScheduler *sched, uint32_t task_id, uint32_t priority)
{
    if (!sched || priority >= MAX_PRIORITY_LEVELS)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id)
        {
            sched->tasks[i].priority = priority;
            return 0;
        }
    }

    return -1;
}

int scheduler_set_cpu_affinity(TaskScheduler *sched, uint32_t task_id, uint32_t cpu_mask)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id)
        {
            sched->tasks[i].cpu_affinity = cpu_mask;
            return 0;
        }
    }

    return -1;
}

int scheduler_enqueue_task(TaskScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    Task *task = NULL;
    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id)
        {
            task = &sched->tasks[i];
            break;
        }
    }

    if (!task)
        return -1;

    task->state = TASK_STATE_READY;
    task->start_time = 0;

    uint32_t target_queue = 0;
    if (task->cpu_affinity != 0xFFFFFFFF)
    {
        for (uint32_t i = 0; i < sched->core_count; i++)
        {
            if (task->cpu_affinity & (1 << i))
            {
                target_queue = i;
                break;
            }
        }
    }
    else
    {
        target_queue = sched->runqueues[0].task_count < sched->runqueues[1].task_count ? 0 : 1;
    }

    if (sched->runqueues[target_queue].task_count < MAX_TASKS)
    {
        sched->runqueues[target_queue].tasks[sched->runqueues[target_queue].task_count++] = task;
        return 0;
    }

    return -1;
}

int scheduler_dequeue_task(TaskScheduler *sched, uint32_t runqueue_id)
{
    if (!sched || runqueue_id >= sched->core_count)
        return -1;

    RunQueue *rq = &sched->runqueues[runqueue_id];
    if (rq->task_count == 0)
        return -1;

    rq->current_task_index = (rq->current_task_index + 1) % rq->task_count;
    return 0;
}

int scheduler_suspend_task(TaskScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id)
        {
            sched->tasks[i].state = TASK_STATE_SUSPENDED;
            return 0;
        }
    }

    return -1;
}

int scheduler_resume_task(TaskScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id && sched->tasks[i].state == TASK_STATE_SUSPENDED)
        {
            sched->tasks[i].state = TASK_STATE_READY;
            return scheduler_enqueue_task(sched, task_id);
        }
    }

    return -1;
}

int scheduler_terminate_task(TaskScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id)
        {
            sched->tasks[i].state = TASK_STATE_TERMINATED;
            sched->tasks[i].end_time = 0;
            return 0;
        }
    }

    return -1;
}

int scheduler_run_scheduling_cycle(TaskScheduler *sched)
{
    if (!sched)
        return -1;

    sched->total_scheduling_decisions++;

    for (uint32_t i = 0; i < sched->core_count; i++)
    {
        if (sched->runqueues[i].task_count > 0)
        {
            Task *current = sched->runqueues[i].tasks[sched->runqueues[i].current_task_index];
            if (current && current->state == TASK_STATE_READY)
            {
                current->state = TASK_STATE_RUNNING;
                current->cpu_time_ms += sched->runqueues[i].time_slice_ms;
            }
        }
    }

    return 0;
}

Task *scheduler_select_next_task(TaskScheduler *sched, uint32_t cpu_core)
{
    if (!sched || cpu_core >= sched->core_count)
        return NULL;

    RunQueue *rq = &sched->runqueues[cpu_core];
    if (rq->task_count == 0)
        return NULL;

    return rq->tasks[rq->current_task_index];
}

int scheduler_context_switch(TaskScheduler *sched, uint32_t from_task_id, uint32_t to_task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == from_task_id)
        {
            sched->tasks[i].state = TASK_STATE_READY;
            sched->tasks[i].context_switches++;
        }
        if (sched->tasks[i].task_id == to_task_id)
        {
            sched->tasks[i].state = TASK_STATE_RUNNING;
            sched->tasks[i].context_switches++;
        }
    }

    sched->total_preemptions++;
    return 0;
}

int scheduler_balance_load(TaskScheduler *sched)
{
    if (!sched)
        return -1;

    return 0;
}

int scheduler_migrate_task(TaskScheduler *sched, uint32_t task_id, uint32_t target_core)
{
    if (!sched || target_core >= sched->core_count)
        return -1;

    return 0;
}

int scheduler_change_policy(TaskScheduler *sched, SchedulingPolicy new_policy)
{
    if (!sched)
        return -1;

    sched->policy = new_policy;
    return 0;
}

int scheduler_set_time_slice(TaskScheduler *sched, uint64_t time_slice_ms)
{
    if (!sched || time_slice_ms == 0)
        return -1;

    for (uint32_t i = 0; i < sched->core_count; i++)
    {
        sched->runqueues[i].time_slice_ms = time_slice_ms;
    }

    return 0;
}

int scheduler_get_metrics(TaskScheduler *sched, SchedulerMetrics *metrics)
{
    if (!sched || !metrics)
        return -1;

    metrics->scheduler_id = sched->scheduler_id;
    metrics->total_tasks = sched->total_tasks;
    metrics->active_tasks = 0;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].state != TASK_STATE_TERMINATED)
        {
            metrics->active_tasks++;
        }
    }

    metrics->average_response_time_ms = sched->average_response_time;
    metrics->average_turnaround_time_ms = sched->average_turnaround_time;
    metrics->total_context_switches = sched->total_preemptions;
    metrics->average_cpu_utilization = 0.0f;

    for (uint32_t i = 0; i < sched->core_count; i++)
    {
        metrics->average_cpu_utilization += sched->runqueues[i].average_utilization;
    }

    metrics->average_cpu_utilization /= sched->core_count;
    metrics->scheduler_efficiency = 0.95f;

    return 0;
}

int scheduler_get_task_metrics(TaskScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->total_tasks; i++)
    {
        if (sched->tasks[i].task_id == task_id)
        {
            return 0;
        }
    }

    return -1;
}
