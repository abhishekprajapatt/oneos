#include "realtime.h"
#include <stdlib.h>
#include <string.h>

RealtimeManager *rt_init(void)
{
    RealtimeManager *manager = (RealtimeManager *)malloc(sizeof(RealtimeManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(RealtimeManager));
    manager->task_count = 0;
    manager->timer_count = 0;
    manager->total_deadline_misses = 0;
    manager->total_preemptions = 0;
    manager->cpu_reservation_percent = 0.0;

    return manager;
}

void rt_cleanup(RealtimeManager *manager)
{
    if (!manager)
        return;

    for (uint32_t i = 0; i < manager->timer_count; i++)
    {
        if (manager->timers[i].is_active)
        {
            rt_stop_timer(manager, manager->timers[i].timer_id);
        }
    }

    memset(manager, 0, sizeof(RealtimeManager));
    free(manager);
}

uint32_t rt_create_task(RealtimeManager *manager, uint32_t task_id, RTClass rt_class,
                        int64_t period_us, int64_t deadline_us, int64_t worst_case_us)
{
    if (!manager || manager->task_count >= MAX_RT_TASKS)
        return 0;
    if (deadline_us <= 0 || worst_case_us <= 0)
        return 0;

    for (uint32_t i = 0; i < manager->task_count; i++)
    {
        if (manager->tasks[i].task_id == task_id)
            return 0;
    }

    RTTask *task = &manager->tasks[manager->task_count];
    task->task_id = task_id;
    task->rt_class = rt_class;
    task->state = RT_STATE_PENDING;
    task->period_us = period_us;
    task->deadline_us = deadline_us;
    task->worst_case_time_us = worst_case_us;
    task->actual_runtime_us = 0;
    task->deadline_misses = 0;
    task->priority = (rt_class == RT_CLASS_HARD) ? 99 : 50;
    task->is_preemptible = (rt_class != RT_CLASS_HARD);
    task->created_time = time(NULL);

    manager->task_count++;

    if (rt_class == RT_CLASS_HARD)
    {
        double util = ((double)worst_case_us / period_us) * 100.0;
        manager->cpu_reservation_percent += util;
    }

    return task_id;
}

int rt_delete_task(RealtimeManager *manager, uint32_t task_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->task_count; i++)
    {
        if (manager->tasks[i].task_id == task_id)
        {
            if (manager->tasks[i].rt_class == RT_CLASS_HARD)
            {
                double util = ((double)manager->tasks[i].worst_case_time_us /
                               manager->tasks[i].period_us) *
                              100.0;
                manager->cpu_reservation_percent -= util;
            }

            memmove(&manager->tasks[i], &manager->tasks[i + 1],
                    (manager->task_count - i - 1) * sizeof(RTTask));
            manager->task_count--;
            return 0;
        }
    }
    return -1;
}

int rt_task_start(RealtimeManager *manager, uint32_t task_id)
{
    if (!manager)
        return -1;

    RTTask *task = rt_get_task(manager, task_id);
    if (!task)
        return -1;

    if (task->state == RT_STATE_PENDING)
    {
        task->state = RT_STATE_READY;
        return 0;
    }
    return -1;
}

int rt_task_complete(RealtimeManager *manager, uint32_t task_id)
{
    if (!manager)
        return -1;

    RTTask *task = rt_get_task(manager, task_id);
    if (!task)
        return -1;

    if (task->state == RT_STATE_RUNNING)
    {
        task->state = RT_STATE_COMPLETED;
        if (task->rt_class == RT_CLASS_HARD)
        {
            task->state = RT_STATE_READY;
        }
        return 0;
    }
    return -1;
}

int rt_task_update_runtime(RealtimeManager *manager, uint32_t task_id, int64_t runtime_us)
{
    if (!manager || runtime_us < 0)
        return -1;

    RTTask *task = rt_get_task(manager, task_id);
    if (!task)
        return -1;

    task->actual_runtime_us += runtime_us;

    if (task->actual_runtime_us > task->deadline_us)
    {
        task->deadline_misses++;
        manager->total_deadline_misses++;
        task->state = RT_STATE_MISSED;
        return -1;
    }
    return 0;
}

int rt_check_deadline(RealtimeManager *manager, uint32_t task_id)
{
    if (!manager)
        return -1;

    RTTask *task = rt_get_task(manager, task_id);
    if (!task)
        return -1;

    if (task->actual_runtime_us > task->deadline_us)
    {
        return 1;
    }
    return 0;
}

int rt_admit_task(RealtimeManager *manager, uint32_t task_id)
{
    if (!manager)
        return -1;

    RTTask *task = rt_get_task(manager, task_id);
    if (!task)
        return -1;

    if (task->rt_class == RT_CLASS_HARD)
    {
        double new_util = ((double)task->worst_case_time_us / task->period_us) * 100.0;
        if (manager->cpu_reservation_percent + new_util > 100.0)
        {
            return -1;
        }
    }

    task->state = RT_STATE_READY;
    return 0;
}

double rt_get_cpu_utilization(RealtimeManager *manager)
{
    if (!manager)
        return 0.0;
    return manager->cpu_reservation_percent;
}

uint32_t rt_create_timer(RealtimeManager *manager, int64_t interval_us,
                         bool periodic, void (*callback)(uint32_t))
{
    if (!manager || manager->timer_count >= MAX_RT_TIMERS)
        return 0;
    if (interval_us <= 0 || !callback)
        return 0;

    RTTimer *timer = &manager->timers[manager->timer_count];
    timer->timer_id = manager->timer_count + 1;
    timer->interval_us = interval_us;
    timer->remaining_us = interval_us;
    timer->is_periodic = periodic;
    timer->is_active = false;
    timer->callback = callback;
    timer->fires = 0;

    manager->timer_count++;
    return timer->timer_id;
}

int rt_start_timer(RealtimeManager *manager, uint32_t timer_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->timer_count; i++)
    {
        if (manager->timers[i].timer_id == timer_id)
        {
            manager->timers[i].is_active = true;
            manager->timers[i].remaining_us = manager->timers[i].interval_us;
            return 0;
        }
    }
    return -1;
}

int rt_stop_timer(RealtimeManager *manager, uint32_t timer_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->timer_count; i++)
    {
        if (manager->timers[i].timer_id == timer_id)
        {
            manager->timers[i].is_active = false;
            return 0;
        }
    }
    return -1;
}

int rt_delete_timer(RealtimeManager *manager, uint32_t timer_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->timer_count; i++)
    {
        if (manager->timers[i].timer_id == timer_id)
        {
            if (manager->timers[i].is_active)
            {
                rt_stop_timer(manager, timer_id);
            }
            memmove(&manager->timers[i], &manager->timers[i + 1],
                    (manager->timer_count - i - 1) * sizeof(RTTimer));
            manager->timer_count--;
            return 0;
        }
    }
    return -1;
}

RTTask *rt_get_task(RealtimeManager *manager, uint32_t task_id)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->task_count; i++)
    {
        if (manager->tasks[i].task_id == task_id)
        {
            return &manager->tasks[i];
        }
    }
    return NULL;
}

uint32_t rt_get_missed_deadlines(RealtimeManager *manager)
{
    if (!manager)
        return 0;
    return (uint32_t)manager->total_deadline_misses;
}

uint64_t rt_get_total_preemptions(RealtimeManager *manager)
{
    if (!manager)
        return 0;
    return manager->total_preemptions;
}
