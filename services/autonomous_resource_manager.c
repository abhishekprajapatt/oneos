#include "autonomous_resource_manager.h"
#include <stdlib.h>
#include <string.h>

static AutonomousResourceManager g_managers[16] = {0};
static uint32_t g_manager_count = 0;

int arm_create_manager(const char *name)
{
    if (g_manager_count >= MAX_RESOURCE_MANAGERS)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[g_manager_count];
    mgr->manager_id = g_manager_count++;
    strncpy(mgr->name, name, 63);
    mgr->name[63] = '\0';

    mgr->pool_count = 0;
    mgr->policy_count = 0;
    mgr->allocation_count = 0;
    mgr->preset_count = 0;

    mgr->default_strategy = ALLOCATION_STRATEGY_BALANCED;
    mgr->total_scaling_actions = 0;
    mgr->successful_allocations = 0;
    mgr->failed_allocations = 0;
    mgr->resource_utilization = 0.0;

    mgr->created_at = time(NULL);
    mgr->last_rebalance = 0;
    mgr->is_active = 1;

    return mgr->manager_id;
}

int arm_delete_manager(uint32_t manager_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    mgr->is_active = 0;
    mgr->pool_count = 0;
    mgr->policy_count = 0;
    mgr->allocation_count = 0;

    return 0;
}

int arm_create_resource_pool(uint32_t manager_id, const char *pool_name,
                             ResourceType resource_type, uint64_t capacity)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (mgr->pool_count >= MAX_RESOURCE_POOLS)
    {
        return -1;
    }

    ResourcePool *pool = &mgr->resource_pools[mgr->pool_count];
    pool->pool_id = mgr->pool_count;
    strncpy(pool->pool_name, pool_name, 63);
    pool->pool_name[63] = '\0';
    pool->resource_type = resource_type;
    pool->total_capacity = capacity;
    pool->allocated_capacity = 0;
    pool->reserved_capacity = 0;
    pool->available_capacity = capacity;
    pool->current_state = RESOURCE_STATE_IDLE;
    pool->health_score = 100;
    pool->created_at = time(NULL);
    pool->is_active = 1;

    mgr->pool_count++;

    return pool->pool_id;
}

int arm_delete_resource_pool(uint32_t manager_id, uint32_t pool_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (pool_id >= mgr->pool_count)
    {
        return -1;
    }

    mgr->resource_pools[pool_id].is_active = 0;

    return 0;
}

int arm_create_scaling_policy(uint32_t manager_id, const char *policy_name,
                              ResourceType resource_type, ScalingMetric metric,
                              double scale_up_threshold, double scale_down_threshold)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (mgr->policy_count >= MAX_AUTO_SCALING_POLICIES)
    {
        return -1;
    }

    AutoScalingPolicy *policy = &mgr->scaling_policies[mgr->policy_count];
    policy->policy_id = mgr->policy_count;
    strncpy(policy->policy_name, policy_name, 63);
    policy->policy_name[63] = '\0';
    policy->resource_type = resource_type;
    policy->metric = metric;
    policy->scale_up_threshold = scale_up_threshold;
    policy->scale_down_threshold = scale_down_threshold;
    policy->scale_up_increment = 10;
    policy->scale_down_decrement = 5;
    policy->cooldown_seconds = 300;
    policy->last_scaling_action = 0;
    policy->scaling_actions_executed = 0;
    policy->is_enabled = 1;

    mgr->policy_count++;

    return policy->policy_id;
}

int arm_enable_scaling_policy(uint32_t manager_id, uint32_t policy_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (policy_id >= mgr->policy_count)
    {
        return -1;
    }

    mgr->scaling_policies[policy_id].is_enabled = 1;

    return 0;
}

int arm_disable_scaling_policy(uint32_t manager_id, uint32_t policy_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (policy_id >= mgr->policy_count)
    {
        return -1;
    }

    mgr->scaling_policies[policy_id].is_enabled = 0;

    return 0;
}

int arm_delete_scaling_policy(uint32_t manager_id, uint32_t policy_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (policy_id >= mgr->policy_count)
    {
        return -1;
    }

    mgr->scaling_policies[policy_id].is_enabled = 0;

    return 0;
}

int arm_allocate_resource(uint32_t manager_id, uint32_t pool_id, uint32_t request_id,
                          uint64_t amount, AllocationStrategy strategy)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (pool_id >= mgr->pool_count)
    {
        return -1;
    }

    ResourcePool *pool = &mgr->resource_pools[pool_id];
    if (pool->available_capacity < amount)
    {
        mgr->failed_allocations++;
        return -1;
    }

    if (mgr->allocation_count >= MAX_ALLOCATION_STRATEGIES)
    {
        mgr->failed_allocations++;
        return -1;
    }

    ResourceAllocation *alloc = &mgr->allocations[mgr->allocation_count];
    alloc->allocation_id = mgr->allocation_count;
    alloc->pool_id = pool_id;
    alloc->request_id = request_id;
    alloc->resource_type = pool->resource_type;
    alloc->requested_amount = amount;
    alloc->allocated_amount = amount;
    alloc->minimum_amount = amount / 2;
    alloc->strategy_used = strategy;
    alloc->allocation_time = time(NULL);
    alloc->deallocation_time = 0;
    alloc->is_active = 1;

    pool->allocated_capacity += amount;
    pool->available_capacity -= amount;

    if (pool->available_capacity < pool->total_capacity / 4)
    {
        pool->current_state = RESOURCE_STATE_OVERUTILIZED;
    }
    else if (pool->allocated_capacity > pool->total_capacity / 2)
    {
        pool->current_state = RESOURCE_STATE_OPTIMALLY_USED;
    }
    else
    {
        pool->current_state = RESOURCE_STATE_ALLOCATED;
    }

    mgr->successful_allocations++;
    mgr->allocation_count++;

    return alloc->allocation_id;
}

int arm_deallocate_resource(uint32_t manager_id, uint64_t allocation_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (allocation_id >= mgr->allocation_count)
    {
        return -1;
    }

    ResourceAllocation *alloc = &mgr->allocations[allocation_id];
    if (!alloc->is_active)
    {
        return -1;
    }

    ResourcePool *pool = &mgr->resource_pools[alloc->pool_id];
    pool->allocated_capacity -= alloc->allocated_amount;
    pool->available_capacity += alloc->allocated_amount;

    alloc->deallocation_time = time(NULL);
    alloc->is_active = 0;

    return 0;
}

int arm_get_allocations(uint32_t manager_id, ResourceAllocation *allocations, uint32_t *count)
{
    if (manager_id >= g_manager_count || !allocations || !count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    uint32_t idx = 0;

    for (uint32_t i = 0; i < mgr->allocation_count && idx < *count; i++)
    {
        if (mgr->allocations[i].is_active)
        {
            allocations[idx++] = mgr->allocations[i];
        }
    }

    *count = idx;

    return 0;
}

int arm_create_resource_preset(uint32_t manager_id, const char *preset_name,
                               uint64_t cpu, uint64_t memory, uint64_t storage)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (mgr->preset_count >= MAX_RESOURCE_PRESETS)
    {
        return -1;
    }

    ResourcePreset *preset = &mgr->presets[mgr->preset_count];
    preset->preset_id = mgr->preset_count;
    strncpy(preset->preset_name, preset_name, 63);
    preset->preset_name[63] = '\0';
    preset->cpu_cores = cpu;
    preset->memory_gb = memory;
    preset->storage_gb = storage;
    preset->network_mbps = 100;
    preset->is_burstable = 0;
    preset->cost_per_hour = (cpu * 10) + (memory * 5) + (storage / 100);
    preset->is_default = 0;

    mgr->preset_count++;

    return preset->preset_id;
}

int arm_delete_resource_preset(uint32_t manager_id, uint32_t preset_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (preset_id >= mgr->preset_count)
    {
        return -1;
    }

    if (preset_id + 1 == mgr->preset_count)
    {
        mgr->preset_count--;
    }

    return 0;
}

int arm_trigger_auto_scaling(uint32_t manager_id, uint32_t policy_id,
                             double current_metric_value)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (policy_id >= mgr->policy_count)
    {
        return -1;
    }

    AutoScalingPolicy *policy = &mgr->scaling_policies[policy_id];
    if (!policy->is_enabled)
    {
        return 0;
    }

    time_t now = time(NULL);
    if ((now - policy->last_scaling_action) < policy->cooldown_seconds)
    {
        return 0;
    }

    if (current_metric_value > policy->scale_up_threshold)
    {
        policy->scaling_actions_executed++;
        mgr->total_scaling_actions++;
        policy->last_scaling_action = now;
        return 1;
    }
    else if (current_metric_value < policy->scale_down_threshold)
    {
        policy->scaling_actions_executed++;
        mgr->total_scaling_actions++;
        policy->last_scaling_action = now;
        return -1;
    }

    return 0;
}

int arm_rebalance_resources(uint32_t manager_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];

    uint64_t total_allocated = 0;
    uint64_t total_capacity = 0;

    for (uint32_t i = 0; i < mgr->pool_count; i++)
    {
        if (mgr->resource_pools[i].is_active)
        {
            total_allocated += mgr->resource_pools[i].allocated_capacity;
            total_capacity += mgr->resource_pools[i].total_capacity;
        }
    }

    if (total_capacity > 0)
    {
        mgr->resource_utilization = (double)total_allocated / (double)total_capacity;
    }

    mgr->last_rebalance = time(NULL);

    return 0;
}

double arm_get_resource_utilization(uint32_t manager_id, ResourceType resource_type)
{
    if (manager_id >= g_manager_count)
    {
        return 0.0;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];

    uint64_t total_allocated = 0;
    uint64_t total_capacity = 0;

    for (uint32_t i = 0; i < mgr->pool_count; i++)
    {
        ResourcePool *pool = &mgr->resource_pools[i];
        if (pool->is_active && pool->resource_type == resource_type)
        {
            total_allocated += pool->allocated_capacity;
            total_capacity += pool->total_capacity;
        }
    }

    if (total_capacity == 0)
    {
        return 0.0;
    }

    return (double)total_allocated / (double)total_capacity;
}

uint32_t arm_get_resource_health(uint32_t manager_id, uint32_t pool_id)
{
    if (manager_id >= g_manager_count)
    {
        return 0;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];
    if (pool_id >= mgr->pool_count)
    {
        return 0;
    }

    return mgr->resource_pools[pool_id].health_score;
}

int arm_perform_autonomous_optimization(uint32_t manager_id)
{
    if (manager_id >= g_manager_count)
    {
        return -1;
    }

    AutonomousResourceManager *mgr = &g_managers[manager_id];

    arm_rebalance_resources(manager_id);

    for (uint32_t i = 0; i < mgr->policy_count; i++)
    {
        if (mgr->scaling_policies[i].is_enabled)
        {
            double utilization = arm_get_resource_utilization(manager_id,
                                                              mgr->scaling_policies[i].resource_type);
            arm_trigger_auto_scaling(manager_id, i, utilization);
        }
    }

    return 0;
}

AutonomousResourceManager *arm_get_manager_info(uint32_t manager_id)
{
    if (manager_id >= g_manager_count)
    {
        return NULL;
    }

    return &g_managers[manager_id];
}
