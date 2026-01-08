#include "resource_allocator.h"
#include <stdlib.h>
#include <string.h>

static ResourceAllocator *global_resource_allocator = NULL;
static uint32_t next_pool_id = 1;
static uint32_t next_request_id = 1;
static uint32_t next_constraint_id = 1;
static uint32_t next_allocation_id = 1;

ResourceAllocator *resource_allocator_init(AllocationStrategy strategy)
{
    ResourceAllocator *allocator = (ResourceAllocator *)malloc(sizeof(ResourceAllocator));
    if (!allocator)
        return NULL;

    memset(allocator, 0, sizeof(ResourceAllocator));
    allocator->allocator_id = 1;
    allocator->pool_count = 0;
    allocator->request_count = 0;
    allocator->constraint_count = 0;
    allocator->allocation_count = 0;
    allocator->strategy = strategy;
    allocator->allocation_failures = 0;
    allocator->successful_allocations = 0;

    global_resource_allocator = allocator;
    return allocator;
}

int resource_allocator_destroy(ResourceAllocator *allocator)
{
    if (!allocator)
        return -1;

    free(allocator);
    global_resource_allocator = NULL;
    return 0;
}

uint32_t resource_pool_create(ResourceAllocator *allocator, ResourceType type, uint64_t capacity)
{
    if (!allocator || allocator->pool_count >= MAX_RESOURCE_POOLS || capacity == 0)
        return 0;

    ResourcePool *pool = &allocator->pools[allocator->pool_count];
    memset(pool, 0, sizeof(ResourcePool));

    pool->pool_id = next_pool_id++;
    pool->resource_type = type;
    pool->total_capacity = capacity;
    pool->available = capacity;
    pool->allocated = 0;
    pool->fragmentation_percent = 0;

    allocator->pool_count++;
    return pool->pool_id;
}

int resource_pool_destroy(ResourceAllocator *allocator, uint32_t pool_id)
{
    if (!allocator || pool_id == 0)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            memmove(&allocator->pools[i], &allocator->pools[i + 1],
                    sizeof(ResourcePool) * (allocator->pool_count - i - 1));
            allocator->pool_count--;
            return 0;
        }
    }

    return -1;
}

uint32_t resource_request_submit(ResourceAllocator *allocator, uint32_t requester_id,
                                 ResourceType resource_type, uint64_t amount, uint32_t priority)
{
    if (!allocator || allocator->request_count >= MAX_RESOURCE_REQUESTS || amount == 0)
        return 0;

    ResourceRequest *request = &allocator->requests[allocator->request_count];
    memset(request, 0, sizeof(ResourceRequest));

    request->request_id = next_request_id++;
    request->requester_id = requester_id;
    request->resource_type = resource_type;
    request->amount_requested = amount;
    request->amount_allocated = 0;
    request->is_fulfilled = 0;
    request->priority = priority;
    request->request_time = 0;

    allocator->request_count++;
    return request->request_id;
}

int resource_request_cancel(ResourceAllocator *allocator, uint32_t request_id)
{
    if (!allocator || request_id == 0)
        return -1;

    for (uint32_t i = 0; i < allocator->request_count; i++)
    {
        if (allocator->requests[i].request_id == request_id)
        {
            memmove(&allocator->requests[i], &allocator->requests[i + 1],
                    sizeof(ResourceRequest) * (allocator->request_count - i - 1));
            allocator->request_count--;
            return 0;
        }
    }

    return -1;
}

int resource_allocate(ResourceAllocator *allocator, uint32_t request_id)
{
    if (!allocator || request_id == 0)
        return -1;

    ResourceRequest *request = NULL;
    for (uint32_t i = 0; i < allocator->request_count; i++)
    {
        if (allocator->requests[i].request_id == request_id)
        {
            request = &allocator->requests[i];
            break;
        }
    }

    if (!request)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].resource_type == request->resource_type)
        {
            ResourcePool *pool = &allocator->pools[i];

            if (allocator->strategy == ALLOCATION_STRATEGY_FIRST_FIT ||
                allocator->strategy == ALLOCATION_STRATEGY_BEST_FIT)
            {

                if (pool->available >= request->amount_requested)
                {
                    ResourceAllocation *allocation = &allocator->allocations[allocator->allocation_count];
                    memset(allocation, 0, sizeof(ResourceAllocation));

                    allocation->allocation_id = next_allocation_id++;
                    allocation->request_id = request_id;
                    allocation->pool_id = pool->pool_id;
                    allocation->allocated_amount = request->amount_requested;
                    allocation->base_address = pool->total_capacity - pool->available;
                    allocation->is_active = 1;
                    allocation->allocation_time = 0;

                    pool->available -= request->amount_requested;
                    pool->allocated += request->amount_requested;
                    request->amount_allocated = request->amount_requested;
                    request->is_fulfilled = 1;

                    allocator->allocation_count++;
                    allocator->successful_allocations++;

                    return allocation->allocation_id;
                }
            }
        }
    }

    allocator->allocation_failures++;
    return -1;
}

int resource_deallocate(ResourceAllocator *allocator, uint32_t allocation_id)
{
    if (!allocator || allocation_id == 0)
        return -1;

    for (uint32_t i = 0; i < allocator->allocation_count; i++)
    {
        if (allocator->allocations[i].allocation_id == allocation_id)
        {
            ResourceAllocation *allocation = &allocator->allocations[i];

            for (uint32_t j = 0; j < allocator->pool_count; j++)
            {
                if (allocator->pools[j].pool_id == allocation->pool_id)
                {
                    ResourcePool *pool = &allocator->pools[j];
                    pool->available += allocation->allocated_amount;
                    pool->allocated -= allocation->allocated_amount;
                    break;
                }
            }

            memmove(&allocator->allocations[i], &allocator->allocations[i + 1],
                    sizeof(ResourceAllocation) * (allocator->allocation_count - i - 1));
            allocator->allocation_count--;
            return 0;
        }
    }

    return -1;
}

int resource_add_constraint(ResourceAllocator *allocator, uint32_t request_id,
                            ConstraintType constraint_type, uint64_t constraint_value)
{
    if (!allocator || allocator->constraint_count >= MAX_RESOURCE_CONSTRAINTS)
        return -1;

    ResourceConstraint *constraint = &allocator->constraints[allocator->constraint_count];
    memset(constraint, 0, sizeof(ResourceConstraint));

    constraint->constraint_id = next_constraint_id++;
    constraint->request_id = request_id;
    constraint->constraint_type = constraint_type;
    constraint->constraint_value = constraint_value;
    constraint->is_enforced = 0;

    allocator->constraint_count++;
    return constraint->constraint_id;
}

int resource_remove_constraint(ResourceAllocator *allocator, uint32_t constraint_id)
{
    if (!allocator || constraint_id == 0)
        return -1;

    for (uint32_t i = 0; i < allocator->constraint_count; i++)
    {
        if (allocator->constraints[i].constraint_id == constraint_id)
        {
            memmove(&allocator->constraints[i], &allocator->constraints[i + 1],
                    sizeof(ResourceConstraint) * (allocator->constraint_count - i - 1));
            allocator->constraint_count--;
            return 0;
        }
    }

    return -1;
}

int resource_validate_constraints(ResourceAllocator *allocator, uint32_t request_id)
{
    if (!allocator || request_id == 0)
        return -1;

    for (uint32_t i = 0; i < allocator->constraint_count; i++)
    {
        if (allocator->constraints[i].request_id == request_id)
        {
            allocator->constraints[i].is_enforced = 1;
        }
    }

    return 0;
}

int resource_defragment_pool(ResourceAllocator *allocator, uint32_t pool_id)
{
    if (!allocator || pool_id == 0)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            allocator->pools[i].fragmentation_percent = 0;
            return 0;
        }
    }

    return -1;
}

int resource_process_pending_requests(ResourceAllocator *allocator)
{
    if (!allocator)
        return 0;

    uint32_t processed = 0;

    for (uint32_t i = 0; i < allocator->request_count; i++)
    {
        if (!allocator->requests[i].is_fulfilled)
        {
            if (resource_allocate(allocator, allocator->requests[i].request_id) >= 0)
            {
                processed++;
            }
        }
    }

    return processed;
}

int resource_rebalance_pools(ResourceAllocator *allocator)
{
    if (!allocator)
        return -1;

    return 0;
}

int resource_update_pool_metrics(ResourceAllocator *allocator, uint32_t pool_id)
{
    if (!allocator || pool_id == 0)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            ResourcePool *pool = &allocator->pools[i];

            if (pool->total_capacity > 0)
            {
                pool->fragmentation_percent = (100 * pool->allocated) / pool->total_capacity;
            }

            return 0;
        }
    }

    return -1;
}

int resource_allocator_get_metrics(ResourceAllocator *allocator, ResourceAllocatorMetrics *metrics)
{
    if (!allocator || !metrics)
        return -1;

    uint64_t total_capacity = 0;
    uint64_t total_allocated = 0;
    uint64_t total_available = 0;
    float total_fragmentation = 0.0f;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        total_capacity += allocator->pools[i].total_capacity;
        total_allocated += allocator->pools[i].allocated;
        total_available += allocator->pools[i].available;
        total_fragmentation += allocator->pools[i].fragmentation_percent;
    }

    metrics->allocator_id = allocator->allocator_id;
    metrics->total_pools = allocator->pool_count;
    metrics->total_capacity = total_capacity;
    metrics->total_allocated = total_allocated;
    metrics->total_available = total_available;
    metrics->allocation_failures = allocator->allocation_failures;

    if (total_capacity > 0)
    {
        metrics->utilization_percent = (float)total_allocated / total_capacity * 100.0f;
    }
    else
    {
        metrics->utilization_percent = 0.0f;
    }

    if (allocator->pool_count > 0)
    {
        metrics->fragmentation_percent = total_fragmentation / allocator->pool_count;
    }
    else
    {
        metrics->fragmentation_percent = 0.0f;
    }

    return 0;
}
