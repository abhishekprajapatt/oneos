#ifndef RESOURCE_ALLOCATOR_H
#define RESOURCE_ALLOCATOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_RESOURCE_POOLS 32
#define MAX_RESOURCE_REQUESTS 2048
#define MAX_ALLOCATED_RESOURCES 8192
#define MAX_RESOURCE_CONSTRAINTS 256

typedef enum
{
    RESOURCE_TYPE_CPU,
    RESOURCE_TYPE_MEMORY,
    RESOURCE_TYPE_DISK,
    RESOURCE_TYPE_NETWORK,
    RESOURCE_TYPE_GPU
} ResourceType;

typedef enum
{
    ALLOCATION_STRATEGY_FIRST_FIT,
    ALLOCATION_STRATEGY_BEST_FIT,
    ALLOCATION_STRATEGY_WORST_FIT,
    ALLOCATION_STRATEGY_OPTIMAL
} AllocationStrategy;

typedef enum
{
    CONSTRAINT_TYPE_MIN_REQUIRED,
    CONSTRAINT_TYPE_MAX_ALLOWED,
    CONSTRAINT_TYPE_AFFINITY,
    CONSTRAINT_TYPE_ANTI_AFFINITY
} ConstraintType;

typedef struct
{
    uint32_t pool_id;
    ResourceType resource_type;

    uint64_t total_capacity;
    uint64_t available;
    uint64_t allocated;
    uint32_t fragmentation_percent;
} ResourcePool;

typedef struct
{
    uint32_t request_id;
    uint32_t requester_id;
    ResourceType resource_type;
    uint64_t amount_requested;
    uint64_t amount_allocated;

    uint8_t is_fulfilled;
    uint32_t priority;
    uint64_t request_time;
} ResourceRequest;

typedef struct
{
    uint32_t constraint_id;
    uint32_t request_id;
    ConstraintType constraint_type;

    uint64_t constraint_value;
    uint8_t is_enforced;
} ResourceConstraint;

typedef struct
{
    uint32_t allocation_id;
    uint32_t request_id;
    uint32_t pool_id;

    uint64_t allocated_amount;
    uint64_t base_address;
    uint32_t is_active;
    uint64_t allocation_time;
} ResourceAllocation;

typedef struct
{
    uint32_t allocator_id;
    ResourcePool pools[MAX_RESOURCE_POOLS];
    uint32_t pool_count;

    ResourceRequest requests[MAX_RESOURCE_REQUESTS];
    uint32_t request_count;

    ResourceConstraint constraints[MAX_RESOURCE_CONSTRAINTS];
    uint32_t constraint_count;

    ResourceAllocation allocations[MAX_ALLOCATED_RESOURCES];
    uint32_t allocation_count;

    AllocationStrategy strategy;
    uint32_t allocation_failures;
    uint32_t successful_allocations;
} ResourceAllocator;

ResourceAllocator *resource_allocator_init(AllocationStrategy strategy);
int resource_allocator_destroy(ResourceAllocator *allocator);

uint32_t resource_pool_create(ResourceAllocator *allocator, ResourceType type, uint64_t capacity);
int resource_pool_destroy(ResourceAllocator *allocator, uint32_t pool_id);

uint32_t resource_request_submit(ResourceAllocator *allocator, uint32_t requester_id,
                                 ResourceType resource_type, uint64_t amount, uint32_t priority);
int resource_request_cancel(ResourceAllocator *allocator, uint32_t request_id);

int resource_allocate(ResourceAllocator *allocator, uint32_t request_id);
int resource_deallocate(ResourceAllocator *allocator, uint32_t allocation_id);

int resource_add_constraint(ResourceAllocator *allocator, uint32_t request_id,
                            ConstraintType constraint_type, uint64_t constraint_value);
int resource_remove_constraint(ResourceAllocator *allocator, uint32_t constraint_id);

int resource_validate_constraints(ResourceAllocator *allocator, uint32_t request_id);
int resource_defragment_pool(ResourceAllocator *allocator, uint32_t pool_id);

int resource_process_pending_requests(ResourceAllocator *allocator);
int resource_rebalance_pools(ResourceAllocator *allocator);

int resource_update_pool_metrics(ResourceAllocator *allocator, uint32_t pool_id);

typedef struct
{
    uint32_t allocator_id;
    uint32_t total_pools;
    uint64_t total_capacity;
    uint64_t total_allocated;
    uint64_t total_available;
    uint32_t allocation_failures;
    float utilization_percent;
    float fragmentation_percent;
} ResourceAllocatorMetrics;

int resource_allocator_get_metrics(ResourceAllocator *allocator, ResourceAllocatorMetrics *metrics);

#endif
