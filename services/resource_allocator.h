#ifndef ONEOS_RESOURCE_ALLOCATOR_H
#define ONEOS_RESOURCE_ALLOCATOR_H

#include <time.h>
#include <stdint.h>

#define MAX_RESOURCE_ALLOCATORS 32
#define MAX_RESOURCE_QUOTAS 512
#define MAX_RESOURCE_LIMITS 512
#define MAX_QOS_POLICIES 256
#define MAX_RESOURCE_SCALING_RULES 256
#define MAX_ALLOCATION_EVENTS 2048

typedef enum
{
    RESOURCE_TYPE_CPU = 0,
    RESOURCE_TYPE_MEMORY = 1,
    RESOURCE_TYPE_DISK_IO = 2,
    RESOURCE_TYPE_NETWORK_IO = 3,
    RESOURCE_TYPE_NETWORK_BANDWIDTH = 4,
    RESOURCE_TYPE_GPU = 5
} ResourceType;

typedef enum
{
    PRIORITY_LEVEL_LOW = 0,
    PRIORITY_LEVEL_NORMAL = 1,
    PRIORITY_LEVEL_HIGH = 2,
    PRIORITY_LEVEL_CRITICAL = 3
} PriorityLevel;

typedef enum
{
    QOS_CLASS_BEST_EFFORT = 0,
    QOS_CLASS_BURSTABLE = 1,
    QOS_CLASS_GUARANTEED = 2,
    QOS_CLASS_ISOLATED = 3
} QosClass;

typedef enum
{
    SCALING_ACTION_INCREASE = 0,
    SCALING_ACTION_DECREASE = 1,
    SCALING_ACTION_MAINTAIN = 2,
    SCALING_ACTION_DYNAMIC = 3
} ScalingAction;

typedef enum
{
    ALLOCATION_STATUS_ALLOCATED = 0,
    ALLOCATION_STATUS_SCALED = 1,
    ALLOCATION_STATUS_THROTTLED = 2,
    ALLOCATION_STATUS_RELEASED = 3,
    ALLOCATION_STATUS_DENIED = 4
} AllocationStatus;

typedef struct
{
    uint32_t quota_id;
    uint32_t process_id;
    ResourceType resource_type;
    uint64_t allocated_amount;
    uint64_t used_amount;
    uint64_t max_allowed;
    uint8_t is_hard_limit;
    time_t created_at;
    time_t last_updated;
    uint32_t exceeded_count;
} ResourceQuota;

typedef struct
{
    uint32_t limit_id;
    uint32_t container_id;
    ResourceType resource_type;
    uint64_t limit_value;
    uint64_t soft_limit_value;
    uint8_t enforce_limit;
    uint8_t kill_on_exceed;
    time_t created_at;
    uint32_t violations_count;
} ResourceLimit;

typedef struct
{
    uint32_t policy_id;
    char policy_name[64];
    QosClass qos_class;
    PriorityLevel priority;
    uint64_t cpu_request;
    uint64_t cpu_limit;
    uint64_t memory_request;
    uint64_t memory_limit;
    uint64_t disk_io_request;
    uint64_t disk_io_limit;
    uint64_t network_bandwidth_limit;
    time_t created_at;
    uint8_t is_active;
} QosPolicy;

typedef struct
{
    uint32_t rule_id;
    char rule_name[64];
    ResourceType resource_type;
    uint64_t trigger_threshold;
    uint64_t target_allocation;
    ScalingAction scaling_action;
    uint32_t cooldown_seconds;
    time_t last_triggered;
    uint8_t is_enabled;
    uint32_t triggered_count;
} ResourceScalingRule;

typedef struct
{
    uint64_t event_id;
    uint32_t allocation_id;
    ResourceType resource_type;
    AllocationStatus status;
    uint64_t requested_amount;
    uint64_t allocated_amount;
    time_t event_time;
    char reason[128];
} AllocationEvent;

typedef struct
{
    uint32_t allocator_id;
    char name[64];
    uint64_t total_cpu_cores;
    uint64_t available_cpu_cores;
    uint64_t total_memory_bytes;
    uint64_t available_memory_bytes;
    uint64_t total_disk_io_bps;
    uint64_t available_disk_io_bps;
    uint64_t total_network_bandwidth_bps;
    uint64_t available_network_bandwidth_bps;

    uint32_t quota_count;
    ResourceQuota quotas[MAX_RESOURCE_QUOTAS];

    uint32_t limit_count;
    ResourceLimit limits[MAX_RESOURCE_LIMITS];

    uint32_t qos_policy_count;
    QosPolicy qos_policies[MAX_QOS_POLICIES];

    uint32_t scaling_rule_count;
    ResourceScalingRule scaling_rules[MAX_RESOURCE_SCALING_RULES];

    uint32_t event_count;
    AllocationEvent events[MAX_ALLOCATION_EVENTS];

    uint64_t total_allocations;
    uint64_t total_denials;
    uint64_t total_scalings;
    uint64_t total_throttles;

    uint8_t is_active;
    time_t created_at;
    time_t last_update;
} ResourceAllocator;

int ra_create_allocator(const char *name, uint64_t total_cpu_cores, uint64_t total_memory_mb);
int ra_delete_allocator(uint32_t allocator_id);

int ra_create_quota(uint32_t allocator_id, uint32_t process_id, ResourceType resource_type,
                    uint64_t allocated_amount, uint64_t max_allowed, uint8_t is_hard_limit);
int ra_update_quota_usage(uint32_t allocator_id, uint32_t quota_id, uint64_t used_amount);
int ra_delete_quota(uint32_t allocator_id, uint32_t quota_id);

int ra_create_limit(uint32_t allocator_id, uint32_t container_id, ResourceType resource_type,
                    uint64_t limit_value, uint8_t kill_on_exceed);
int ra_enforce_resource_limit(uint32_t allocator_id, uint32_t limit_id);
int ra_delete_limit(uint32_t allocator_id, uint32_t limit_id);

int ra_create_qos_policy(uint32_t allocator_id, const char *policy_name, QosClass qos_class,
                         PriorityLevel priority, uint64_t cpu_request, uint64_t memory_request);
int ra_apply_qos_policy(uint32_t allocator_id, uint32_t policy_id, uint32_t process_id);
int ra_delete_qos_policy(uint32_t allocator_id, uint32_t policy_id);

int ra_add_scaling_rule(uint32_t allocator_id, const char *rule_name, ResourceType resource_type,
                        uint64_t trigger_threshold, ScalingAction scaling_action);
int ra_trigger_scaling_rule(uint32_t allocator_id, uint32_t rule_id);
int ra_delete_scaling_rule(uint32_t allocator_id, uint32_t rule_id);

int ra_allocate_resource(uint32_t allocator_id, uint32_t process_id, ResourceType resource_type,
                         uint64_t requested_amount);
int ra_release_resource(uint32_t allocator_id, uint32_t process_id, ResourceType resource_type,
                        uint64_t amount);
int ra_rebalance_resources(uint32_t allocator_id);

uint64_t ra_get_available_cpu(uint32_t allocator_id);
uint64_t ra_get_available_memory(uint32_t allocator_id);
uint64_t ra_get_available_disk_io(uint32_t allocator_id);
uint64_t ra_get_available_network_bandwidth(uint32_t allocator_id);

uint64_t ra_get_total_allocations(uint32_t allocator_id);
uint64_t ra_get_total_denials(uint32_t allocator_id);
uint64_t ra_get_total_scalings(uint32_t allocator_id);
uint64_t ra_get_total_throttles(uint32_t allocator_id);

int ra_get_allocation_events(uint32_t allocator_id, AllocationEvent *events, uint32_t *count);
int ra_get_qos_policies(uint32_t allocator_id, QosPolicy *policies, uint32_t *count);
int ra_get_resource_quotas(uint32_t allocator_id, ResourceQuota *quotas, uint32_t *count);

ResourceAllocator *ra_get_allocator_info(uint32_t allocator_id);

#endif
