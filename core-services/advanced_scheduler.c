#include "advanced_scheduler.h"
#include <stdlib.h>
#include <string.h>

AdvancedScheduler *scheduler_init(uint32_t core_count)
{
    if (core_count == 0 || core_count > MAX_CPU_CORES)
        return NULL;

    AdvancedScheduler *sched = (AdvancedScheduler *)malloc(sizeof(AdvancedScheduler));
    if (!sched)
        return NULL;

    memset(sched->cores, 0, MAX_CPU_CORES * sizeof(CPUCore));
    memset(sched->domains, 0, MAX_SCHEDULING_DOMAINS * sizeof(SchedulingDomain));
    memset(sched->task_table, 0, 4096 * sizeof(TaskInfo));

    sched->core_count = core_count;
    sched->domain_count = 0;
    sched->task_count = 0;
    sched->scheduling_events = 0;
    sched->avg_load = 0.0;

    for (uint32_t i = 0; i < core_count; i++)
    {
        sched->cores[i].core_id = i;
        sched->cores[i].task_count = 0;
        sched->cores[i].total_time = 0;
        sched->cores[i].idle_time = 0;
        sched->cores[i].utilization = 0.0;
        sched->cores[i].current_task_id = 0;
        sched->cores[i].context_switches = 0;
    }

    return sched;
}

void scheduler_cleanup(AdvancedScheduler *sched)
{
    if (!sched)
        return;
    free(sched);
}

uint32_t scheduler_add_task(AdvancedScheduler *sched, uint32_t task_id,
                            uint32_t cpu_affinity, SchedulingClass class, int deadline)
{
    if (!sched || sched->task_count >= 4096)
        return 0;

    TaskInfo *task = &sched->task_table[sched->task_count];
    task->task_id = task_id;
    task->cpu_affinity_mask = cpu_affinity;
    task->nice = 0;
    task->deadline_ms = deadline;
    task->sched_class = class;
    task->cpu_time_ms = 0;
    task->preemption_count = 0;
    task->is_pinned = false;
    task->preferred_core = 0;

    sched->task_count++;
    return task_id;
}

int scheduler_remove_task(AdvancedScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->task_count; i++)
    {
        if (sched->task_table[i].task_id == task_id)
        {
            memmove(&sched->task_table[i], &sched->task_table[i + 1],
                    (sched->task_count - i - 1) * sizeof(TaskInfo));
            sched->task_count--;
            return 0;
        }
    }

    return -1;
}

int scheduler_set_affinity(AdvancedScheduler *sched, uint32_t task_id, uint32_t affinity_mask)
{
    if (!sched)
        return -1;

    for (uint32_t i = 0; i < sched->task_count; i++)
    {
        if (sched->task_table[i].task_id == task_id)
        {
            sched->task_table[i].cpu_affinity_mask = affinity_mask;
            return 0;
        }
    }

    return -1;
}

int scheduler_pin_task(AdvancedScheduler *sched, uint32_t task_id, uint32_t core)
{
    if (!sched || core >= sched->core_count)
        return -1;

    for (uint32_t i = 0; i < sched->task_count; i++)
    {
        if (sched->task_table[i].task_id == task_id)
        {
            sched->task_table[i].is_pinned = true;
            sched->task_table[i].preferred_core = core;
            return 0;
        }
    }

    return -1;
}

int scheduler_load_balance(AdvancedScheduler *sched)
{
    if (!sched || sched->core_count == 0)
        return -1;

    uint32_t min_load = UINT32_MAX;
    uint32_t max_load = 0;

    for (uint32_t i = 0; i < sched->core_count; i++)
    {
        if (sched->cores[i].task_count < min_load)
        {
            min_load = sched->cores[i].task_count;
        }
        if (sched->cores[i].task_count > max_load)
        {
            max_load = sched->cores[i].task_count;
        }
    }

    if (max_load - min_load > sched->core_count / 2)
    {
        sched->scheduling_events++;
    }

    return 0;
}

uint32_t scheduler_select_cpu(AdvancedScheduler *sched, uint32_t task_id)
{
    if (!sched || sched->core_count == 0)
        return 0;

    for (uint32_t i = 0; i < sched->task_count; i++)
    {
        if (sched->task_table[i].task_id == task_id)
        {
            TaskInfo *task = &sched->task_table[i];

            if (task->is_pinned)
            {
                return task->preferred_core;
            }

            uint32_t best_core = 0;
            uint32_t min_tasks = UINT32_MAX;

            for (uint32_t j = 0; j < sched->core_count; j++)
            {
                if ((task->cpu_affinity_mask & (1 << j)) && sched->cores[j].task_count < min_tasks)
                {
                    min_tasks = sched->cores[j].task_count;
                    best_core = j;
                }
            }

            return best_core;
        }
    }

    return 0;
}

int scheduler_create_domain(AdvancedScheduler *sched, uint32_t core_mask)
{
    if (!sched || sched->domain_count >= MAX_SCHEDULING_DOMAINS)
        return -1;

    SchedulingDomain *domain = &sched->domains[sched->domain_count];
    domain->domain_id = sched->domain_count + 1;
    domain->core_mask = core_mask;
    domain->core_count = __builtin_popcount(core_mask);
    domain->task_count = 0;
    domain->load_balance_factor = 1.0;

    memset(domain->tasks, 0, MAX_TASKS_PER_DOMAIN * sizeof(TaskInfo));

    sched->domain_count++;
    return domain->domain_id;
}

int scheduler_update_load(AdvancedScheduler *sched)
{
    if (!sched)
        return -1;

    uint64_t total_load = 0;

    for (uint32_t i = 0; i < sched->core_count; i++)
    {
        sched->cores[i].utilization = (double)sched->cores[i].task_count / sched->task_count;
        total_load += sched->cores[i].task_count;
    }

    sched->avg_load = (double)total_load / sched->core_count;

    return 0;
}

double scheduler_get_cpu_load(AdvancedScheduler *sched, uint32_t core_id)
{
    if (!sched || core_id >= sched->core_count)
        return 0.0;
    return sched->cores[core_id].utilization;
}

double scheduler_get_system_load(AdvancedScheduler *sched)
{
    if (!sched)
        return 0.0;
    return sched->avg_load;
}

TaskInfo *scheduler_get_task_info(AdvancedScheduler *sched, uint32_t task_id)
{
    if (!sched)
        return NULL;

    for (uint32_t i = 0; i < sched->task_count; i++)
    {
        if (sched->task_table[i].task_id == task_id)
        {
            return &sched->task_table[i];
        }
    }

    return NULL;
}

CPUCore *scheduler_get_core_info(AdvancedScheduler *sched, uint32_t core_id)
{
    if (!sched || core_id >= sched->core_count)
        return NULL;
    return &sched->cores[core_id];
}

uint32_t scheduler_get_task_count(AdvancedScheduler *sched)
{
    if (!sched)
        return 0;
    return sched->task_count;
}
