#include "memory_pool_allocator.h"
#include <stdlib.h>
#include <string.h>

static MemoryAllocator *global_allocator = NULL;
static uint32_t next_pool_id = 1;
static uint32_t next_block_id = 1;

MemoryAllocator *allocator_init(uint64_t total_memory)
{
    MemoryAllocator *allocator = (MemoryAllocator *)malloc(sizeof(MemoryAllocator));
    if (!allocator)
        return NULL;

    memset(allocator, 0, sizeof(MemoryAllocator));
    allocator->allocator_id = 1;
    allocator->pool_count = 0;
    allocator->total_memory_managed = total_memory;
    allocator->total_allocated = 0;
    allocator->total_free = total_memory;
    allocator->total_allocations = 0;
    allocator->total_deallocations = 0;
    allocator->average_fragmentation = 0.0f;

    global_allocator = allocator;
    return allocator;
}

int allocator_destroy(MemoryAllocator *allocator)
{
    if (!allocator)
        return -1;

    free(allocator);
    global_allocator = NULL;
    return 0;
}

MemoryPool *allocator_create_pool(MemoryAllocator *allocator, PoolType type, uint64_t pool_size)
{
    if (!allocator || allocator->pool_count >= MAX_MEMORY_POOLS || pool_size > MAX_POOL_SIZE)
        return NULL;

    MemoryPool *pool = &allocator->pools[allocator->pool_count];
    memset(pool, 0, sizeof(MemoryPool));

    pool->pool_id = next_pool_id++;
    pool->pool_type = type;
    pool->pool_size = pool_size;
    pool->pool_base = malloc(pool_size);

    if (!pool->pool_base)
        return NULL;

    pool->block_count = 0;
    pool->total_allocated = 0;
    pool->total_free = pool_size;
    pool->peak_usage = 0;
    pool->allocation_count = 0;
    pool->deallocation_count = 0;
    pool->fragmentation_ratio = 0.0f;

    allocator->pool_count++;
    allocator->total_free -= pool_size;

    return pool;
}

int allocator_destroy_pool(MemoryAllocator *allocator, uint32_t pool_id)
{
    if (!allocator)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            free(allocator->pools[i].pool_base);
            allocator->total_free += allocator->pools[i].pool_size;
            memset(&allocator->pools[i], 0, sizeof(MemoryPool));
            return 0;
        }
    }

    return -1;
}

void *allocator_allocate(MemoryAllocator *allocator, uint32_t pool_id, uint64_t size)
{
    if (!allocator || size == 0)
        return NULL;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            MemoryPool *pool = &allocator->pools[i];

            if (pool->total_free < size)
                return NULL;

            if (pool->block_count >= 4096)
                return NULL;

            MemoryBlock *block = &pool->blocks[pool->block_count];
            memset(block, 0, sizeof(MemoryBlock));

            block->block_id = next_block_id++;
            block->block_ptr = (void *)((uint8_t *)pool->pool_base + pool->total_allocated);
            block->block_size = size;
            block->is_allocated = 1;
            block->allocations = 1;
            block->allocation_time = 0;

            pool->block_count++;
            pool->total_allocated += size;
            pool->total_free -= size;
            pool->allocation_count++;

            if (pool->total_allocated > pool->peak_usage)
            {
                pool->peak_usage = pool->total_allocated;
            }

            allocator->total_allocated += size;
            allocator->total_allocations++;

            return block->block_ptr;
        }
    }

    return NULL;
}

int allocator_deallocate(MemoryAllocator *allocator, uint32_t pool_id, void *ptr)
{
    if (!allocator || !ptr)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            MemoryPool *pool = &allocator->pools[i];

            for (uint32_t j = 0; j < pool->block_count; j++)
            {
                if (pool->blocks[j].block_ptr == ptr && pool->blocks[j].is_allocated)
                {
                    pool->total_allocated -= pool->blocks[j].block_size;
                    pool->total_free += pool->blocks[j].block_size;
                    pool->deallocation_count++;

                    allocator->total_allocated -= pool->blocks[j].block_size;
                    allocator->total_deallocations++;

                    memset(&pool->blocks[j], 0, sizeof(MemoryBlock));
                    return 0;
                }
            }
        }
    }

    return -1;
}

int allocator_resize_block(MemoryAllocator *allocator, uint32_t pool_id, void *ptr, uint64_t new_size)
{
    if (!allocator || !ptr || new_size == 0)
        return -1;

    return 0;
}

int allocator_defragment_pool(MemoryAllocator *allocator, uint32_t pool_id)
{
    if (!allocator)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            MemoryPool *pool = &allocator->pools[i];

            uint32_t free_blocks = 0;
            for (uint32_t j = 0; j < pool->block_count; j++)
            {
                if (!pool->blocks[j].is_allocated)
                {
                    free_blocks++;
                }
            }

            if (free_blocks > 0)
            {
                pool->fragmentation_ratio = (float)free_blocks / pool->block_count;
            }

            return 0;
        }
    }

    return -1;
}

int allocator_compact_pool(MemoryAllocator *allocator, uint32_t pool_id)
{
    if (!allocator)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            return allocator_defragment_pool(allocator, pool_id);
        }
    }

    return -1;
}

int allocator_enable_statistics(MemoryAllocator *allocator)
{
    if (!allocator)
        return -1;

    return 0;
}

int allocator_get_pool_statistics(MemoryAllocator *allocator, uint32_t pool_id)
{
    if (!allocator)
        return -1;

    for (uint32_t i = 0; i < allocator->pool_count; i++)
    {
        if (allocator->pools[i].pool_id == pool_id)
        {
            return 0;
        }
    }

    return -1;
}

int allocator_get_metrics(MemoryAllocator *allocator, AllocatorMetrics *metrics)
{
    if (!allocator || !metrics)
        return -1;

    metrics->allocator_id = allocator->allocator_id;
    metrics->total_memory = allocator->total_memory_managed;
    metrics->used_memory = allocator->total_allocated;
    metrics->free_memory = allocator->total_free;

    if (allocator->total_memory_managed > 0)
    {
        metrics->utilization_percent = (float)allocator->total_allocated * 100.0f / allocator->total_memory_managed;
    }
    else
    {
        metrics->utilization_percent = 0.0f;
    }

    metrics->fragmentation_ratio = allocator->average_fragmentation;
    metrics->total_allocations = allocator->total_allocations;

    return 0;
}
