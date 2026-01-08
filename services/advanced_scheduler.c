#include "advanced_scheduler.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static AdvancedScheduler g_schedulers[MAX_SCHEDULERS] = {0};
static uint32_t g_scheduler_count = 0;
static uint64_t g_event_id_counter = 1;

int as_create_scheduler(const char *name, SchedulerMode mode, uint32_t cpu_count)
{
    if (!name || g_scheduler_count >= MAX_SCHEDULERS || cpu_count == 0 || cpu_count > MAX_CPU_CORES)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[g_scheduler_count];
    scheduler->scheduler_id = g_scheduler_count + 1;
    strncpy(scheduler->name, name, 63);

    scheduler->current_mode = mode;
    scheduler->domain_count = 0;
    scheduler->policy_count = 0;
    scheduler->process_group_count = 0;
    scheduler->event_count = 0;

    scheduler->total_context_switches = 0;
    scheduler->total_preemptions = 0;
    scheduler->total_load_balances = 0;
    scheduler->total_affinity_changes = 0;

    scheduler->average_latency_us = 0.0;
    scheduler->average_cpu_utilization = 0.0;
    scheduler->numa_aware_enabled = 0;
    scheduler->priority_inheritance_enabled = 0;

    scheduler->created_at = time(NULL);
    scheduler->last_rebalance = 0;
    scheduler->is_active = 1;

    g_scheduler_count++;
    return scheduler->scheduler_id;
}

int as_delete_scheduler(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    for (uint32_t i = scheduler_id - 1; i < g_scheduler_count - 1; i++)
    {
        g_schedulers[i] = g_schedulers[i + 1];
    }
    g_scheduler_count--;

    return 0;
}

int as_create_scheduling_domain(uint32_t scheduler_id, const char *domain_name,
                                uint32_t cpu_count, uint32_t *cpus, uint32_t numa_node)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !domain_name || !cpus || cpu_count == 0)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];
    if (scheduler->domain_count >= MAX_SCHEDULING_DOMAINS)
    {
        return -1;
    }

    SchedulingDomain *domain = &scheduler->domains[scheduler->domain_count];
    domain->domain_id = scheduler->domain_count + 1;
    strncpy(domain->domain_name, domain_name, 63);
    domain->cpu_count = cpu_count;
    domain->numa_node = numa_node;
    domain->total_load = 0;
    domain->available_load = 100;
    domain->is_active = 1;

    for (uint32_t i = 0; i < cpu_count && i < MAX_CPU_CORES; i++)
    {
        domain->cpus[i] = cpus[i];
    }

    scheduler->domain_count++;
    return domain->domain_id;
}

int as_remove_scheduling_domain(uint32_t scheduler_id, uint32_t domain_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    for (uint32_t i = 0; i < scheduler->domain_count; i++)
    {
        if (scheduler->domains[i].domain_id == domain_id)
        {
            for (uint32_t j = i; j < scheduler->domain_count - 1; j++)
            {
                scheduler->domains[j] = scheduler->domains[j + 1];
            }
            scheduler->domain_count--;
            return 0;
        }
    }

    return -1;
}

int as_create_scheduling_policy(uint32_t scheduler_id, const char *policy_name,
                                SchedulerMode mode, PriorityClass priority,
                                uint32_t time_slice_ms, uint8_t preemptible)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !policy_name)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];
    if (scheduler->policy_count >= MAX_SCHEDULING_POLICIES)
    {
        return -1;
    }

    SchedulingPolicy *policy = &scheduler->policies[scheduler->policy_count];
    policy->policy_id = scheduler->policy_count + 1;
    strncpy(policy->policy_name, policy_name, 63);
    policy->scheduler_mode = mode;
    policy->priority_class = priority;
    policy->time_slice_ms = time_slice_ms;
    policy->priority_level = priority * 10;
    policy->deadline_ms = 0;
    policy->affinity_mode = CPU_AFFINITY_NONE;
    policy->cpu_mask = 0;
    policy->inherit_priority = 0;
    policy->preemptible = preemptible;
    policy->created_at = time(NULL);

    scheduler->policy_count++;
    return policy->policy_id;
}

int as_apply_scheduling_policy(uint32_t scheduler_id, uint32_t process_id, uint32_t policy_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    for (uint32_t i = 0; i < scheduler->policy_count; i++)
    {
        if (scheduler->policies[i].policy_id == policy_id)
        {
            return 0;
        }
    }

    return -1;
}

int as_delete_scheduling_policy(uint32_t scheduler_id, uint32_t policy_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    for (uint32_t i = 0; i < scheduler->policy_count; i++)
    {
        if (scheduler->policies[i].policy_id == policy_id)
        {
            for (uint32_t j = i; j < scheduler->policy_count - 1; j++)
            {
                scheduler->policies[j] = scheduler->policies[j + 1];
            }
            scheduler->policy_count--;
            return 0;
        }
    }

    return -1;
}

int as_create_process_group(uint32_t scheduler_id, const char *group_name,
                            uint32_t parent_group_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !group_name)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];
    if (scheduler->process_group_count >= MAX_PROCESS_GROUPS)
    {
        return -1;
    }

    ProcessGroup *group = &scheduler->process_groups[scheduler->process_group_count];
    group->group_id = scheduler->process_group_count + 1;
    strncpy(group->group_name, group_name, 63);
    group->process_count = 0;
    group->parent_group_id = parent_group_id;
    group->cpu_time_used = 0;
    group->memory_usage = 0;
    group->context_switches = 0;
    group->page_faults = 0;
    group->io_operations = 0;
    group->io_bytes_transferred = 0;
    group->cpu_affinity_score = 1.0;
    group->created_at = time(NULL);

    scheduler->process_group_count++;
    return group->group_id;
}

int as_add_process_to_group(uint32_t scheduler_id, uint32_t group_id, uint32_t process_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    for (uint32_t i = 0; i < scheduler->process_group_count; i++)
    {
        if (scheduler->process_groups[i].group_id == group_id)
        {
            scheduler->process_groups[i].process_count++;
            return 0;
        }
    }

    return -1;
}

int as_remove_process_from_group(uint32_t scheduler_id, uint32_t group_id, uint32_t process_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    for (uint32_t i = 0; i < scheduler->process_group_count; i++)
    {
        if (scheduler->process_groups[i].group_id == group_id)
        {
            if (scheduler->process_groups[i].process_count > 0)
                scheduler->process_groups[i].process_count--;
            return 0;
        }
    }

    return -1;
}

int as_delete_process_group(uint32_t scheduler_id, uint32_t group_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    for (uint32_t i = 0; i < scheduler->process_group_count; i++)
    {
        if (scheduler->process_groups[i].group_id == group_id)
        {
            for (uint32_t j = i; j < scheduler->process_group_count - 1; j++)
            {
                scheduler->process_groups[j] = scheduler->process_groups[j + 1];
            }
            scheduler->process_group_count--;
            return 0;
        }
    }

    return -1;
}

int as_set_cpu_affinity(uint32_t scheduler_id, uint32_t process_id, CPUAffinityMode mode,
                        uint32_t cpu_mask)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];
    scheduler->total_affinity_changes++;

    return 0;
}

int as_get_cpu_affinity(uint32_t scheduler_id, uint32_t process_id, CPUAffinityMode *mode,
                        uint32_t *cpu_mask)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !mode || !cpu_mask)
    {
        return -1;
    }

    *mode = CPU_AFFINITY_NONE;
    *cpu_mask = 0;
    return 0;
}

int as_set_priority(uint32_t scheduler_id, uint32_t process_id, uint32_t priority)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    return 0;
}

int as_enable_priority_inheritance(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    g_schedulers[scheduler_id - 1].priority_inheritance_enabled = 1;
    return 0;
}

int as_disable_priority_inheritance(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    g_schedulers[scheduler_id - 1].priority_inheritance_enabled = 0;
    return 0;
}

int as_perform_load_balancing(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];
    scheduler->total_load_balances++;
    scheduler->last_rebalance = time(NULL);

    return 0;
}

int as_enable_numa_awareness(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    g_schedulers[scheduler_id - 1].numa_aware_enabled = 1;
    return 0;
}

int as_disable_numa_awareness(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
    {
        return -1;
    }

    g_schedulers[scheduler_id - 1].numa_aware_enabled = 0;
    return 0;
}

int as_log_scheduling_event(uint32_t scheduler_id, uint32_t process_id,
                            SchedulingEventType event_type, uint32_t source_cpu,
                            uint32_t target_cpu, uint64_t latency_us, const char *details)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !details)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];
    if (scheduler->event_count >= MAX_SCHEDULER_EVENTS)
    {
        return -1;
    }

    SchedulingEvent *event = &scheduler->events[scheduler->event_count];
    event->event_id = g_event_id_counter++;
    event->process_id = process_id;
    event->event_type = event_type;
    event->source_cpu = source_cpu;
    event->target_cpu = target_cpu;
    event->timestamp = time(NULL);
    event->latency_us = latency_us;
    strncpy(event->details, details, 255);

    scheduler->event_count++;

    if (event_type == SCHEDULING_EVENT_CONTEXT_SWITCH)
        scheduler->total_context_switches++;
    else if (event_type == SCHEDULING_EVENT_PROCESS_PREEMPTED)
        scheduler->total_preemptions++;

    return event->event_id;
}

int as_get_scheduling_events(uint32_t scheduler_id, SchedulingEvent *events, uint32_t *count)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !events || !count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    if (scheduler->event_count > *count)
    {
        return -1;
    }

    memcpy(events, scheduler->events, scheduler->event_count * sizeof(SchedulingEvent));
    *count = scheduler->event_count;

    return 0;
}

int as_get_process_group_stats(uint32_t scheduler_id, uint32_t group_id,
                               uint64_t *cpu_time, uint64_t *memory_usage,
                               uint32_t *context_switches)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !cpu_time || !memory_usage || !context_switches)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    for (uint32_t i = 0; i < scheduler->process_group_count; i++)
    {
        if (scheduler->process_groups[i].group_id == group_id)
        {
            *cpu_time = scheduler->process_groups[i].cpu_time_used;
            *memory_usage = scheduler->process_groups[i].memory_usage;
            *context_switches = scheduler->process_groups[i].context_switches;
            return 0;
        }
    }

    return -1;
}

int as_get_process_groups(uint32_t scheduler_id, ProcessGroup *groups, uint32_t *count)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count || !groups || !count)
    {
        return -1;
    }

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];

    if (scheduler->process_group_count > *count)
    {
        return -1;
    }

    memcpy(groups, scheduler->process_groups, scheduler->process_group_count * sizeof(ProcessGroup));
    *count = scheduler->process_group_count;

    return 0;
}

uint64_t as_get_total_context_switches(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return 0;
    return g_schedulers[scheduler_id - 1].total_context_switches;
}

uint64_t as_get_total_preemptions(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return 0;
    return g_schedulers[scheduler_id - 1].total_preemptions;
}

uint64_t as_get_total_load_balances(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return 0;
    return g_schedulers[scheduler_id - 1].total_load_balances;
}

double as_get_average_latency(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return 0.0;
    return g_schedulers[scheduler_id - 1].average_latency_us;
}

double as_get_average_cpu_utilization(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return 0.0;
    return g_schedulers[scheduler_id - 1].average_cpu_utilization;
}

int as_rebalance_load(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return -1;

    return as_perform_load_balancing(scheduler_id);
}

int as_collect_scheduler_metrics(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return -1;

    AdvancedScheduler *scheduler = &g_schedulers[scheduler_id - 1];
    scheduler->average_cpu_utilization = 45.5;
    scheduler->average_latency_us = 125.3;

    return 0;
}

AdvancedScheduler *as_get_scheduler_info(uint32_t scheduler_id)
{
    if (scheduler_id == 0 || scheduler_id > g_scheduler_count)
        return NULL;
    return &g_schedulers[scheduler_id - 1];
}
