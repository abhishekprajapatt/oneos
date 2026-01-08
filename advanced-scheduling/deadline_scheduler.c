#include "deadline_scheduler.h"
#include <stdlib.h>
#include <string.h>

static DeadlineScheduler *global_deadline_sched = NULL;
static uint32_t next_deadline_task_id = 1;

DeadlineScheduler *deadline_scheduler_init(void)
{
    DeadlineScheduler *sched = (DeadlineScheduler *)malloc(sizeof(DeadlineScheduler));
    if (!sched)
        return NULL;

    memset(sched, 0, sizeof(DeadlineScheduler));
    sched->scheduler_id = 1;
    sched->periodic_task_count = 0;
    sched->aperiodic_task_count = 0;
    sched->total_deadline_tasks = 0;
    sched->tasks_completed_on_time = 0;
    sched->tasks_missed_deadline = 0;
    sched->deadline_miss_ratio = 0.0f;
    sched->average_slack_time = 0.0f;
    sched->cpu_utilization = 0.0f;

    global_deadline_sched = sched;
    return sched;
}

int deadline_scheduler_destroy(DeadlineScheduler *sched)
{
    if (!sched)
        return -1;

    free(sched);
    global_deadline_sched = NULL;
    return 0;
}

DeadlineTask *deadline_add_periodic_task(DeadlineScheduler *sched, uint64_t arrival, uint64_t deadline, uint64_t period, uint64_t computation)
{
    if (!sched || sched->periodic_task_count >= MAX_PERIODIC_TASKS)
        return NULL;

    DeadlineTask *task = &sched->periodic_tasks[sched->periodic_task_count];
    memset(task, 0, sizeof(DeadlineTask));

    task->deadline_task_id = next_deadline_task_id++;
    task->task_type = TASK_PERIODIC;
    task->arrival_time = arrival;
    task->absolute_deadline = arrival + deadline;
    task->relative_deadline = deadline;
    task->period = period;
    task->computation_time = computation;
    task->actual_start_time = 0;
    task->actual_end_time = 0;
    task->is_feasible = 1;
    task->is_completed = 0;
    task->missed_deadline = 0;
    task->priority = 0;

    sched->periodic_task_count++;
    sched->total_deadline_tasks++;

    return task;
}

DeadlineTask *deadline_add_aperiodic_task(DeadlineScheduler *sched, uint64_t arrival, uint64_t deadline, uint64_t computation)
{
    if (!sched || sched->aperiodic_task_count >= MAX_APERIODIC_TASKS)
        return NULL;

    DeadlineTask *task = &sched->aperiodic_tasks[sched->aperiodic_task_count];
    memset(task, 0, sizeof(DeadlineTask));

    task->deadline_task_id = next_deadline_task_id++;
    task->task_type = TASK_APERIODIC;
    task->arrival_time = arrival;
    task->absolute_deadline = arrival + deadline;
    task->relative_deadline = deadline;
    task->period = 0;
    task->computation_time = computation;
    task->actual_start_time = 0;
    task->actual_end_time = 0;
    task->is_feasible = 1;
    task->is_completed = 0;
    task->missed_deadline = 0;
    task->priority = 0;

    sched->aperiodic_task_count++;
    sched->total_deadline_tasks++;

    return task;
}

int deadline_scheduler_run_cycle(DeadlineScheduler *sched)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->periodic_task_count; i++)
    {
        DeadlineTask *task = &sched->periodic_tasks[i];
        if (!task->is_completed)
        {
            task->actual_start_time = 0;
            task->actual_end_time = task->actual_start_time + task->computation_time;
        }
    }

    for (uint32_t i = 0; i < sched->aperiodic_task_count; i++)
    {
        DeadlineTask *task = &sched->aperiodic_tasks[i];
        if (!task->is_completed)
        {
            task->actual_start_time = 0;
            task->actual_end_time = task->actual_start_time + task->computation_time;
        }
    }

    return 0;
}

int deadline_check_feasibility(DeadlineScheduler *sched)
{
    if (!sched)
        return -1;

    float total_utilization = 0.0f;

    for (uint32_t i = 0; i < sched->periodic_task_count; i++)
    {
        DeadlineTask *task = &sched->periodic_tasks[i];
        if (task->period > 0)
        {
            float utilization = (float)task->computation_time / task->period;
            total_utilization += utilization;
        }
    }

    sched->cpu_utilization = total_utilization;

    if (total_utilization > 1.0f)
    {
        for (uint32_t i = 0; i < sched->periodic_task_count; i++)
        {
            sched->periodic_tasks[i].is_feasible = 0;
        }
        return -1;
    }

    return 0;
}

int deadline_update_task_status(DeadlineScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->periodic_task_count; i++)
    {
        if (sched->periodic_tasks[i].deadline_task_id == task_id)
        {
            sched->periodic_tasks[i].is_completed = 1;
            return 0;
        }
    }

    for (uint32_t i = 0; i < sched->aperiodic_task_count; i++)
    {
        if (sched->aperiodic_tasks[i].deadline_task_id == task_id)
        {
            sched->aperiodic_tasks[i].is_completed = 1;
            return 0;
        }
    }

    return -1;
}

int deadline_compute_slack_time(DeadlineScheduler *sched)
{
    if (!sched)
        return -1;

    uint64_t total_slack = 0;
    uint32_t count = 0;

    for (uint32_t i = 0; i < sched->periodic_task_count; i++)
    {
        DeadlineTask *task = &sched->periodic_tasks[i];
        uint64_t slack = (task->absolute_deadline > task->actual_end_time) ? (task->absolute_deadline - task->actual_end_time) : 0;
        total_slack += slack;
        count++;
    }

    for (uint32_t i = 0; i < sched->aperiodic_task_count; i++)
    {
        DeadlineTask *task = &sched->aperiodic_tasks[i];
        uint64_t slack = (task->absolute_deadline > task->actual_end_time) ? (task->absolute_deadline - task->actual_end_time) : 0;
        total_slack += slack;
        count++;
    }

    if (count > 0)
    {
        sched->average_slack_time = (float)total_slack / count;
    }

    return 0;
}

int deadline_compute_laxity(DeadlineTask *task, uint64_t current_time)
{
    if (!task)
        return -1;

    int64_t laxity = (int64_t)(task->absolute_deadline - current_time) - (int64_t)task->computation_time;
    return (laxity > 0) ? (int)laxity : 0;
}

int deadline_detect_deadline_miss(DeadlineScheduler *sched)
{
    if (!sched)
        return -1;

    uint64_t current_time = 0;

    for (uint32_t i = 0; i < sched->periodic_task_count; i++)
    {
        DeadlineTask *task = &sched->periodic_tasks[i];
        if (task->actual_end_time > task->absolute_deadline)
        {
            task->missed_deadline = 1;
            sched->tasks_missed_deadline++;
        }
    }

    for (uint32_t i = 0; i < sched->aperiodic_task_count; i++)
    {
        DeadlineTask *task = &sched->aperiodic_tasks[i];
        if (task->actual_end_time > task->absolute_deadline)
        {
            task->missed_deadline = 1;
            sched->tasks_missed_deadline++;
        }
    }

    if (sched->total_deadline_tasks > 0)
    {
        sched->deadline_miss_ratio = (float)sched->tasks_missed_deadline / sched->total_deadline_tasks;
    }

    return 0;
}

int deadline_get_metrics(DeadlineScheduler *sched, DeadlineMetrics *metrics)
{
    if (!sched || !metrics)
        return -1;

    metrics->scheduler_id = sched->scheduler_id;
    metrics->total_tasks = sched->total_deadline_tasks;
    metrics->tasks_on_time = sched->tasks_completed_on_time;
    metrics->tasks_missed = sched->tasks_missed_deadline;
    metrics->miss_ratio = sched->deadline_miss_ratio;
    metrics->average_slack = sched->average_slack_time;
    metrics->utilization = sched->cpu_utilization;

    return 0;
}
