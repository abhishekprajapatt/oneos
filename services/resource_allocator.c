#include "resource_allocator.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static ResourceAllocator g_allocators[MAX_RESOURCE_ALLOCATORS] = {0};
static uint32_t g_allocator_count = 0;
static uint64_t g_event_id_counter = 1;

int ra_create_allocator(const char *name, uint64_t total_cpu_cores, uint64_t total_memory_mb)
{
    if (!name || g_allocator_count >= MAX_RESOURCE_ALLOCATORS)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[g_allocator_count];
    allocator->allocator_id = g_allocator_count + 1;
    strncpy(allocator->name, name, 63);

    allocator->total_cpu_cores = total_cpu_cores;
    allocator->available_cpu_cores = total_cpu_cores;
    allocator->total_memory_bytes = total_memory_mb * 1024 * 1024;
    allocator->available_memory_bytes = allocator->total_memory_bytes;
    allocator->total_disk_io_bps = 1024 * 1024 * 1024;
    allocator->available_disk_io_bps = allocator->total_disk_io_bps;
    allocator->total_network_bandwidth_bps = 10 * 1024 * 1024 * 1024;
    allocator->available_network_bandwidth_bps = allocator->total_network_bandwidth_bps;

    allocator->quota_count = 0;
    allocator->limit_count = 0;
    allocator->qos_policy_count = 0;
    allocator->scaling_rule_count = 0;
    allocator->event_count = 0;

    allocator->total_allocations = 0;
    allocator->total_denials = 0;
    allocator->total_scalings = 0;
    allocator->total_throttles = 0;

    allocator->is_active = 1;
    allocator->created_at = time(NULL);
    allocator->last_update = time(NULL);

    g_allocator_count++;
    return allocator->allocator_id;
}

int ra_delete_allocator(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    for (uint32_t i = allocator_id - 1; i < g_allocator_count - 1; i++)
    {
        g_allocators[i] = g_allocators[i + 1];
    }
    g_allocator_count--;

    return 0;
}

int ra_create_quota(uint32_t allocator_id, uint32_t process_id, ResourceType resource_type,
                    uint64_t allocated_amount, uint64_t max_allowed, uint8_t is_hard_limit)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];
    if (allocator->quota_count >= MAX_RESOURCE_QUOTAS)
    {
        return -1;
    }

    ResourceQuota *quota = &allocator->quotas[allocator->quota_count];
    quota->quota_id = allocator->quota_count + 1;
    quota->process_id = process_id;
    quota->resource_type = resource_type;
    quota->allocated_amount = allocated_amount;
    quota->used_amount = 0;
    quota->max_allowed = max_allowed;
    quota->is_hard_limit = is_hard_limit;
    quota->created_at = time(NULL);
    quota->last_updated = time(NULL);
    quota->exceeded_count = 0;

    allocator->quota_count++;
    allocator->total_allocations++;

    return quota->quota_id;
}

int ra_update_quota_usage(uint32_t allocator_id, uint32_t quota_id, uint64_t used_amount)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->quota_count; i++)
    {
        if (allocator->quotas[i].quota_id == quota_id)
        {
            allocator->quotas[i].used_amount = used_amount;
            allocator->quotas[i].last_updated = time(NULL);

            if (used_amount > allocator->quotas[i].max_allowed)
            {
                allocator->quotas[i].exceeded_count++;
                allocator->total_throttles++;
            }

            return 0;
        }
    }

    return -1;
}

int ra_delete_quota(uint32_t allocator_id, uint32_t quota_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->quota_count; i++)
    {
        if (allocator->quotas[i].quota_id == quota_id)
        {
            for (uint32_t j = i; j < allocator->quota_count - 1; j++)
            {
                allocator->quotas[j] = allocator->quotas[j + 1];
            }
            allocator->quota_count--;
            return 0;
        }
    }

    return -1;
}

int ra_create_limit(uint32_t allocator_id, uint32_t container_id, ResourceType resource_type,
                    uint64_t limit_value, uint8_t kill_on_exceed)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];
    if (allocator->limit_count >= MAX_RESOURCE_LIMITS)
    {
        return -1;
    }

    ResourceLimit *limit = &allocator->limits[allocator->limit_count];
    limit->limit_id = allocator->limit_count + 1;
    limit->container_id = container_id;
    limit->resource_type = resource_type;
    limit->limit_value = limit_value;
    limit->soft_limit_value = (limit_value * 80) / 100;
    limit->enforce_limit = 0;
    limit->kill_on_exceed = kill_on_exceed;
    limit->created_at = time(NULL);
    limit->violations_count = 0;

    allocator->limit_count++;

    return limit->limit_id;
}

int ra_enforce_resource_limit(uint32_t allocator_id, uint32_t limit_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->limit_count; i++)
    {
        if (allocator->limits[i].limit_id == limit_id)
        {
            allocator->limits[i].enforce_limit = 1;
            return 0;
        }
    }

    return -1;
}

int ra_delete_limit(uint32_t allocator_id, uint32_t limit_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->limit_count; i++)
    {
        if (allocator->limits[i].limit_id == limit_id)
        {
            for (uint32_t j = i; j < allocator->limit_count - 1; j++)
            {
                allocator->limits[j] = allocator->limits[j + 1];
            }
            allocator->limit_count--;
            return 0;
        }
    }

    return -1;
}

int ra_create_qos_policy(uint32_t allocator_id, const char *policy_name, QosClass qos_class,
                         PriorityLevel priority, uint64_t cpu_request, uint64_t memory_request)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count || !policy_name)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];
    if (allocator->qos_policy_count >= MAX_QOS_POLICIES)
    {
        return -1;
    }

    QosPolicy *policy = &allocator->qos_policies[allocator->qos_policy_count];
    policy->policy_id = allocator->qos_policy_count + 1;
    strncpy(policy->policy_name, policy_name, 63);
    policy->qos_class = qos_class;
    policy->priority = priority;
    policy->cpu_request = cpu_request;
    policy->cpu_limit = cpu_request * 2;
    policy->memory_request = memory_request;
    policy->memory_limit = memory_request * 2;
    policy->disk_io_request = 1024 * 1024;
    policy->disk_io_limit = 10 * 1024 * 1024;
    policy->network_bandwidth_limit = 100 * 1024 * 1024;
    policy->created_at = time(NULL);
    policy->is_active = 1;

    allocator->qos_policy_count++;

    return policy->policy_id;
}

int ra_apply_qos_policy(uint32_t allocator_id, uint32_t policy_id, uint32_t process_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->qos_policy_count; i++)
    {
        if (allocator->qos_policies[i].policy_id == policy_id)
        {
            allocator->qos_policies[i].is_active = 1;
            return 0;
        }
    }

    return -1;
}

int ra_delete_qos_policy(uint32_t allocator_id, uint32_t policy_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->qos_policy_count; i++)
    {
        if (allocator->qos_policies[i].policy_id == policy_id)
        {
            for (uint32_t j = i; j < allocator->qos_policy_count - 1; j++)
            {
                allocator->qos_policies[j] = allocator->qos_policies[j + 1];
            }
            allocator->qos_policy_count--;
            return 0;
        }
    }

    return -1;
}

int ra_add_scaling_rule(uint32_t allocator_id, const char *rule_name, ResourceType resource_type,
                        uint64_t trigger_threshold, ScalingAction scaling_action)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count || !rule_name)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];
    if (allocator->scaling_rule_count >= MAX_RESOURCE_SCALING_RULES)
    {
        return -1;
    }

    ResourceScalingRule *rule = &allocator->scaling_rules[allocator->scaling_rule_count];
    rule->rule_id = allocator->scaling_rule_count + 1;
    strncpy(rule->rule_name, rule_name, 63);
    rule->resource_type = resource_type;
    rule->trigger_threshold = trigger_threshold;
    rule->target_allocation = (trigger_threshold * 120) / 100;
    rule->scaling_action = scaling_action;
    rule->cooldown_seconds = 300;
    rule->last_triggered = 0;
    rule->is_enabled = 1;
    rule->triggered_count = 0;

    allocator->scaling_rule_count++;

    return rule->rule_id;
}

int ra_trigger_scaling_rule(uint32_t allocator_id, uint32_t rule_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->scaling_rule_count; i++)
    {
        if (allocator->scaling_rules[i].rule_id == rule_id && allocator->scaling_rules[i].is_enabled)
        {
            time_t now = time(NULL);
            if ((now - allocator->scaling_rules[i].last_triggered) >= allocator->scaling_rules[i].cooldown_seconds)
            {
                allocator->scaling_rules[i].last_triggered = now;
                allocator->scaling_rules[i].triggered_count++;
                allocator->total_scalings++;
                return 0;
            }

            return -1;
        }
    }

    return -1;
}

int ra_delete_scaling_rule(uint32_t allocator_id, uint32_t rule_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    for (uint32_t i = 0; i < allocator->scaling_rule_count; i++)
    {
        if (allocator->scaling_rules[i].rule_id == rule_id)
        {
            for (uint32_t j = i; j < allocator->scaling_rule_count - 1; j++)
            {
                allocator->scaling_rules[j] = allocator->scaling_rules[j + 1];
            }
            allocator->scaling_rule_count--;
            return 0;
        }
    }

    return -1;
}

int ra_allocate_resource(uint32_t allocator_id, uint32_t process_id, ResourceType resource_type,
                         uint64_t requested_amount)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    AllocationEvent *event = &allocator->events[allocator->event_count];
    if (allocator->event_count >= MAX_ALLOCATION_EVENTS)
    {
        return -1;
    }

    event->event_id = g_event_id_counter++;
    event->allocation_id = allocator->event_count;
    event->resource_type = resource_type;
    event->requested_amount = requested_amount;
    event->event_time = time(NULL);

    uint64_t available = 0;

    switch (resource_type)
    {
    case RESOURCE_TYPE_CPU:
        available = allocator->available_cpu_cores;
        if (available >= requested_amount)
        {
            allocator->available_cpu_cores -= requested_amount;
            event->allocated_amount = requested_amount;
            event->status = ALLOCATION_STATUS_ALLOCATED;
            strcpy(event->reason, "CPU allocated successfully");
            allocator->total_allocations++;
        }
        else
        {
            event->allocated_amount = 0;
            event->status = ALLOCATION_STATUS_DENIED;
            strcpy(event->reason, "Insufficient CPU cores");
            allocator->total_denials++;
        }
        break;

    case RESOURCE_TYPE_MEMORY:
        available = allocator->available_memory_bytes;
        if (available >= requested_amount)
        {
            allocator->available_memory_bytes -= requested_amount;
            event->allocated_amount = requested_amount;
            event->status = ALLOCATION_STATUS_ALLOCATED;
            strcpy(event->reason, "Memory allocated successfully");
            allocator->total_allocations++;
        }
        else
        {
            event->allocated_amount = 0;
            event->status = ALLOCATION_STATUS_DENIED;
            strcpy(event->reason, "Insufficient memory");
            allocator->total_denials++;
        }
        break;

    case RESOURCE_TYPE_DISK_IO:
        available = allocator->available_disk_io_bps;
        if (available >= requested_amount)
        {
            allocator->available_disk_io_bps -= requested_amount;
            event->allocated_amount = requested_amount;
            event->status = ALLOCATION_STATUS_ALLOCATED;
            strcpy(event->reason, "Disk I/O allocated successfully");
            allocator->total_allocations++;
        }
        else
        {
            event->allocated_amount = 0;
            event->status = ALLOCATION_STATUS_DENIED;
            strcpy(event->reason, "Insufficient disk I/O bandwidth");
            allocator->total_denials++;
        }
        break;

    case RESOURCE_TYPE_NETWORK_IO:
    case RESOURCE_TYPE_NETWORK_BANDWIDTH:
        available = allocator->available_network_bandwidth_bps;
        if (available >= requested_amount)
        {
            allocator->available_network_bandwidth_bps -= requested_amount;
            event->allocated_amount = requested_amount;
            event->status = ALLOCATION_STATUS_ALLOCATED;
            strcpy(event->reason, "Network bandwidth allocated successfully");
            allocator->total_allocations++;
        }
        else
        {
            event->allocated_amount = 0;
            event->status = ALLOCATION_STATUS_DENIED;
            strcpy(event->reason, "Insufficient network bandwidth");
            allocator->total_denials++;
        }
        break;

    default:
        event->allocated_amount = 0;
        event->status = ALLOCATION_STATUS_DENIED;
        strcpy(event->reason, "Unknown resource type");
        allocator->total_denials++;
        break;
    }

    allocator->event_count++;
    allocator->last_update = time(NULL);

    return (event->status == ALLOCATION_STATUS_ALLOCATED) ? 0 : -1;
}

int ra_release_resource(uint32_t allocator_id, uint32_t process_id, ResourceType resource_type,
                        uint64_t amount)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    switch (resource_type)
    {
    case RESOURCE_TYPE_CPU:
        allocator->available_cpu_cores += amount;
        if (allocator->available_cpu_cores > allocator->total_cpu_cores)
            allocator->available_cpu_cores = allocator->total_cpu_cores;
        break;

    case RESOURCE_TYPE_MEMORY:
        allocator->available_memory_bytes += amount;
        if (allocator->available_memory_bytes > allocator->total_memory_bytes)
            allocator->available_memory_bytes = allocator->total_memory_bytes;
        break;

    case RESOURCE_TYPE_DISK_IO:
        allocator->available_disk_io_bps += amount;
        if (allocator->available_disk_io_bps > allocator->total_disk_io_bps)
            allocator->available_disk_io_bps = allocator->total_disk_io_bps;
        break;

    case RESOURCE_TYPE_NETWORK_IO:
    case RESOURCE_TYPE_NETWORK_BANDWIDTH:
        allocator->available_network_bandwidth_bps += amount;
        if (allocator->available_network_bandwidth_bps > allocator->total_network_bandwidth_bps)
            allocator->available_network_bandwidth_bps = allocator->total_network_bandwidth_bps;
        break;

    default:
        return -1;
    }

    allocator->last_update = time(NULL);
    return 0;
}

int ra_rebalance_resources(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return -1;
    }

    allocator_id--;
    return 0;
}

uint64_t ra_get_available_cpu(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].available_cpu_cores;
}

uint64_t ra_get_available_memory(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].available_memory_bytes;
}

uint64_t ra_get_available_disk_io(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].available_disk_io_bps;
}

uint64_t ra_get_available_network_bandwidth(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].available_network_bandwidth_bps;
}

uint64_t ra_get_total_allocations(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].total_allocations;
}

uint64_t ra_get_total_denials(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].total_denials;
}

uint64_t ra_get_total_scalings(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].total_scalings;
}

uint64_t ra_get_total_throttles(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return 0;
    }

    return g_allocators[allocator_id - 1].total_throttles;
}

int ra_get_allocation_events(uint32_t allocator_id, AllocationEvent *events, uint32_t *count)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count || !events || !count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    if (allocator->event_count > *count)
    {
        return -1;
    }

    memcpy(events, allocator->events, allocator->event_count * sizeof(AllocationEvent));
    *count = allocator->event_count;

    return 0;
}

int ra_get_qos_policies(uint32_t allocator_id, QosPolicy *policies, uint32_t *count)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count || !policies || !count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    if (allocator->qos_policy_count > *count)
    {
        return -1;
    }

    memcpy(policies, allocator->qos_policies, allocator->qos_policy_count * sizeof(QosPolicy));
    *count = allocator->qos_policy_count;

    return 0;
}

int ra_get_resource_quotas(uint32_t allocator_id, ResourceQuota *quotas, uint32_t *count)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count || !quotas || !count)
    {
        return -1;
    }

    ResourceAllocator *allocator = &g_allocators[allocator_id - 1];

    if (allocator->quota_count > *count)
    {
        return -1;
    }

    memcpy(quotas, allocator->quotas, allocator->quota_count * sizeof(ResourceQuota));
    *count = allocator->quota_count;

    return 0;
}

ResourceAllocator *ra_get_allocator_info(uint32_t allocator_id)
{
    if (allocator_id == 0 || allocator_id > g_allocator_count)
    {
        return NULL;
    }

    return &g_allocators[allocator_id - 1];
}
