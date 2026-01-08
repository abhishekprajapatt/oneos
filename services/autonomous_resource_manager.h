#ifndef ONEOS_AUTONOMOUS_RESOURCE_MANAGER_H
#define ONEOS_AUTONOMOUS_RESOURCE_MANAGER_H

#include <stdint.h>
#include <time.h>

#define MAX_RESOURCE_MANAGERS 16
#define MAX_RESOURCE_POOLS 128
#define MAX_AUTO_SCALING_POLICIES 256
#define MAX_ALLOCATION_STRATEGIES 512
#define MAX_RESOURCE_PRESETS 64

typedef enum
{
    RESOURCE_TYPE_CPU = 0,
    RESOURCE_TYPE_MEMORY = 1,
    RESOURCE_TYPE_STORAGE = 2,
    RESOURCE_TYPE_NETWORK = 3,
    RESOURCE_TYPE_GPU = 4
} ResourceType;

typedef enum
{
    SCALING_DIRECTION_UP = 0,
    SCALING_DIRECTION_DOWN = 1,
    SCALING_DIRECTION_NONE = 2
} ScalingDirection;

typedef enum
{
    SCALING_METRIC_CPU = 0,
    SCALING_METRIC_MEMORY = 1,
    SCALING_METRIC_RESPONSE_TIME = 2,
    SCALING_METRIC_QUEUE_LENGTH = 3,
    SCALING_METRIC_THROUGHPUT = 4,
    SCALING_METRIC_ERROR_RATE = 5
} ScalingMetric;

typedef enum
{
    ALLOCATION_STRATEGY_BEST_FIT = 0,
    ALLOCATION_STRATEGY_FIRST_FIT = 1,
    ALLOCATION_STRATEGY_WORST_FIT = 2,
    ALLOCATION_STRATEGY_BALANCED = 3,
    ALLOCATION_STRATEGY_POWER_OPTIMIZED = 4
} AllocationStrategy;

typedef enum
{
    RESOURCE_STATE_IDLE = 0,
    RESOURCE_STATE_ALLOCATED = 1,
    RESOURCE_STATE_UNDERUTILIZED = 2,
    RESOURCE_STATE_OPTIMALLY_USED = 3,
    RESOURCE_STATE_OVERUTILIZED = 4,
    RESOURCE_STATE_RESERVED = 5
} ResourceState;

typedef struct
{
    uint32_t pool_id;
    char pool_name[64];
    ResourceType resource_type;
    uint64_t total_capacity;
    uint64_t allocated_capacity;
    uint64_t reserved_capacity;
    uint64_t available_capacity;
    ResourceState current_state;
    uint32_t health_score;
    time_t created_at;
    uint8_t is_active;
} ResourcePool;

typedef struct
{
    uint32_t policy_id;
    char policy_name[64];
    ResourceType resource_type;
    ScalingMetric metric;
    double scale_up_threshold;
    double scale_down_threshold;
    uint32_t scale_up_increment;
    uint32_t scale_down_decrement;
    uint32_t cooldown_seconds;
    time_t last_scaling_action;
    uint32_t scaling_actions_executed;
    uint8_t is_enabled;
} AutoScalingPolicy;

typedef struct
{
    uint64_t allocation_id;
    uint32_t pool_id;
    uint32_t request_id;
    ResourceType resource_type;
    uint64_t requested_amount;
    uint64_t allocated_amount;
    uint64_t minimum_amount;
    AllocationStrategy strategy_used;
    time_t allocation_time;
    time_t deallocation_time;
    uint8_t is_active;
} ResourceAllocation;

typedef struct
{
    uint32_t preset_id;
    char preset_name[64];
    ResourceType resource_type;
    uint64_t cpu_cores;
    uint64_t memory_gb;
    uint64_t storage_gb;
    uint64_t network_mbps;
    uint8_t is_burstable;
    uint32_t cost_per_hour;
    uint8_t is_default;
} ResourcePreset;

typedef struct
{
    uint32_t manager_id;
    char name[64];

    uint32_t pool_count;
    ResourcePool resource_pools[128];

    uint32_t policy_count;
    AutoScalingPolicy scaling_policies[256];

    uint32_t allocation_count;
    ResourceAllocation allocations[512];

    uint32_t preset_count;
    ResourcePreset presets[64];

    AllocationStrategy default_strategy;
    uint64_t total_scaling_actions;
    uint64_t successful_allocations;
    uint64_t failed_allocations;
    double resource_utilization;

    time_t created_at;
    time_t last_rebalance;
    uint8_t is_active;
} AutonomousResourceManager;

int arm_create_manager(const char *name);
int arm_delete_manager(uint32_t manager_id);

int arm_create_resource_pool(uint32_t manager_id, const char *pool_name,
                             ResourceType resource_type, uint64_t capacity);
int arm_delete_resource_pool(uint32_t manager_id, uint32_t pool_id);

int arm_create_scaling_policy(uint32_t manager_id, const char *policy_name,
                              ResourceType resource_type, ScalingMetric metric,
                              double scale_up_threshold, double scale_down_threshold);
int arm_enable_scaling_policy(uint32_t manager_id, uint32_t policy_id);
int arm_disable_scaling_policy(uint32_t manager_id, uint32_t policy_id);
int arm_delete_scaling_policy(uint32_t manager_id, uint32_t policy_id);

int arm_allocate_resource(uint32_t manager_id, uint32_t pool_id, uint32_t request_id,
                          uint64_t amount, AllocationStrategy strategy);
int arm_deallocate_resource(uint32_t manager_id, uint64_t allocation_id);
int arm_get_allocations(uint32_t manager_id, ResourceAllocation *allocations, uint32_t *count);

int arm_create_resource_preset(uint32_t manager_id, const char *preset_name,
                               uint64_t cpu, uint64_t memory, uint64_t storage);
int arm_delete_resource_preset(uint32_t manager_id, uint32_t preset_id);

int arm_trigger_auto_scaling(uint32_t manager_id, uint32_t policy_id,
                             double current_metric_value);
int arm_rebalance_resources(uint32_t manager_id);

double arm_get_resource_utilization(uint32_t manager_id, ResourceType resource_type);
uint32_t arm_get_resource_health(uint32_t manager_id, uint32_t pool_id);

int arm_perform_autonomous_optimization(uint32_t manager_id);
AutonomousResourceManager *arm_get_manager_info(uint32_t manager_id);

#endif
