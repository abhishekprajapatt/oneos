#include "disk_cache_buffer.h"
#include <stdlib.h>
#include <string.h>

static DiskCacheBuffer *global_disk_cache = NULL;
static uint32_t next_block_id = 1;

DiskCacheBuffer *disk_cache_init(uint64_t buffer_size, uint32_t pool_count)
{
    DiskCacheBuffer *cache = (DiskCacheBuffer *)malloc(sizeof(DiskCacheBuffer));
    if (!cache)
        return NULL;

    memset(cache, 0, sizeof(DiskCacheBuffer));
    cache->cache_manager_id = 1;
    cache->pool_count = (pool_count > MAX_BUFFER_POOLS) ? MAX_BUFFER_POOLS : pool_count;
    cache->total_buffer_size = buffer_size;
    cache->allocated_buffer_size = 0;
    cache->total_reads = 0;
    cache->total_writes = 0;
    cache->total_flushes = 0;
    cache->total_hits = 0;
    cache->total_misses = 0;
    cache->hit_ratio = 0.0f;
    cache->utilization_ratio = 0.0f;

    uint64_t pool_size = buffer_size / cache->pool_count;

    for (uint32_t i = 0; i < cache->pool_count; i++)
    {
        cache->pools[i].pool_id = i;
        cache->pools[i].block_count = pool_size / BUFFER_BLOCK_SIZE;
        cache->pools[i].blocks = (BufferBlock *)malloc(sizeof(BufferBlock) * cache->pools[i].block_count);

        if (!cache->pools[i].blocks)
        {
            free(cache);
            return NULL;
        }

        memset(cache->pools[i].blocks, 0, sizeof(BufferBlock) * cache->pools[i].block_count);
        cache->pools[i].total_pool_size = pool_size;
        cache->pools[i].allocated_size = 0;
        cache->pools[i].dirty_count = 0;
        cache->pools[i].hit_count = 0;
        cache->pools[i].miss_count = 0;
    }

    global_disk_cache = cache;
    return cache;
}

int disk_cache_destroy(DiskCacheBuffer *cache)
{
    if (!cache)
        return -1;

    for (uint32_t i = 0; i < cache->pool_count; i++)
    {
        if (cache->pools[i].blocks)
        {
            for (uint32_t j = 0; j < cache->pools[i].block_count; j++)
            {
                if (cache->pools[i].blocks[j].block_ptr)
                    free(cache->pools[i].blocks[j].block_ptr);
            }
            free(cache->pools[i].blocks);
        }
    }

    free(cache);
    global_disk_cache = NULL;
    return 0;
}

BufferBlock *disk_cache_allocate_block(DiskCacheBuffer *cache, uint32_t pool_id)
{
    if (!cache || pool_id >= cache->pool_count)
        return NULL;

    BufferPool *pool = &cache->pools[pool_id];

    for (uint32_t i = 0; i < pool->block_count; i++)
    {
        if (pool->blocks[i].state == BUFFER_FREE)
        {
            BufferBlock *block = &pool->blocks[i];

            block->block_id = next_block_id++;
            block->block_ptr = malloc(BUFFER_BLOCK_SIZE);
            if (!block->block_ptr)
                return NULL;

            block->state = BUFFER_ALLOCATED;
            block->reference_count = 1;
            block->is_dirty = 0;
            block->is_pinned = 0;
            block->allocation_time = 0;
            block->last_access_time = 0;
            block->access_count = 0;

            pool->allocated_size += BUFFER_BLOCK_SIZE;
            cache->allocated_buffer_size += BUFFER_BLOCK_SIZE;

            return block;
        }
    }

    return NULL;
}

int disk_cache_deallocate_block(DiskCacheBuffer *cache, uint32_t pool_id, uint32_t block_id)
{
    if (!cache || pool_id >= cache->pool_count)
        return -1;

    BufferPool *pool = &cache->pools[pool_id];

    for (uint32_t i = 0; i < pool->block_count; i++)
    {
        if (pool->blocks[i].block_id == block_id && pool->blocks[i].state != BUFFER_FREE)
        {
            BufferBlock *block = &pool->blocks[i];

            if (block->is_pinned)
                return -1;

            free(block->block_ptr);
            pool->allocated_size -= BUFFER_BLOCK_SIZE;
            cache->allocated_buffer_size -= BUFFER_BLOCK_SIZE;

            if (block->is_dirty)
            {
                pool->dirty_count--;
            }

            memset(block, 0, sizeof(BufferBlock));
            return 0;
        }
    }

    return -1;
}

BufferBlock *disk_cache_read_block(DiskCacheBuffer *cache, uint64_t disk_location)
{
    if (!cache)
        return NULL;

    for (uint32_t i = 0; i < cache->pool_count; i++)
    {
        BufferPool *pool = &cache->pools[i];
        for (uint32_t j = 0; j < pool->block_count; j++)
        {
            if (pool->blocks[j].disk_location == disk_location && pool->blocks[j].state != BUFFER_FREE)
            {
                pool->blocks[j].access_count++;
                pool->blocks[j].last_access_time = 0;
                pool->hit_count++;
                cache->total_hits++;
                return &pool->blocks[j];
            }
        }
    }

    cache->total_misses++;
    cache->total_reads++;
    return NULL;
}

int disk_cache_write_block(DiskCacheBuffer *cache, uint64_t disk_location, const uint8_t *data)
{
    if (!cache || !data)
        return -1;

    BufferBlock *block = disk_cache_read_block(cache, disk_location);

    if (block)
    {
        memcpy(block->block_ptr, data, BUFFER_BLOCK_SIZE);
        block->state = BUFFER_DIRTY;
        block->is_dirty = 1;
        cache->total_writes++;
        return 0;
    }

    BufferBlock *new_block = disk_cache_allocate_block(cache, 0);
    if (!new_block)
        return -1;

    memcpy(new_block->block_ptr, data, BUFFER_BLOCK_SIZE);
    new_block->disk_location = disk_location;
    new_block->state = BUFFER_DIRTY;
    new_block->is_dirty = 1;
    cache->total_writes++;

    return 0;
}

int disk_cache_flush_blocks(DiskCacheBuffer *cache, uint32_t pool_id)
{
    if (!cache || pool_id >= cache->pool_count)
        return -1;

    BufferPool *pool = &cache->pools[pool_id];
    uint32_t flushed = 0;

    for (uint32_t i = 0; i < pool->block_count; i++)
    {
        if (pool->blocks[i].is_dirty && !pool->blocks[i].is_pinned)
        {
            pool->blocks[i].state = BUFFER_ALLOCATED;
            pool->blocks[i].is_dirty = 0;
            pool->dirty_count--;
            flushed++;
        }
    }

    cache->total_flushes += flushed;
    return flushed;
}

int disk_cache_flush_all(DiskCacheBuffer *cache)
{
    if (!cache)
        return -1;

    uint32_t total_flushed = 0;

    for (uint32_t i = 0; i < cache->pool_count; i++)
    {
        total_flushed += disk_cache_flush_blocks(cache, i);
    }

    return total_flushed;
}

int disk_cache_lock_block(DiskCacheBuffer *cache, uint32_t block_id)
{
    if (!cache)
        return -1;

    for (uint32_t i = 0; i < cache->pool_count; i++)
    {
        for (uint32_t j = 0; j < cache->pools[i].block_count; j++)
        {
            if (cache->pools[i].blocks[j].block_id == block_id)
            {
                cache->pools[i].blocks[j].is_pinned = 1;
                return 0;
            }
        }
    }

    return -1;
}

int disk_cache_unlock_block(DiskCacheBuffer *cache, uint32_t block_id)
{
    if (!cache)
        return -1;

    for (uint32_t i = 0; i < cache->pool_count; i++)
    {
        for (uint32_t j = 0; j < cache->pools[i].block_count; j++)
        {
            if (cache->pools[i].blocks[j].block_id == block_id)
            {
                cache->pools[i].blocks[j].is_pinned = 0;
                return 0;
            }
        }
    }

    return -1;
}

int disk_cache_prefetch_blocks(DiskCacheBuffer *cache, uint64_t start_location, uint32_t count)
{
    if (!cache || count == 0)
        return -1;

    for (uint32_t i = 0; i < count; i++)
    {
        uint8_t dummy_data[BUFFER_BLOCK_SIZE] = {0};
        disk_cache_write_block(cache, start_location + (i * BUFFER_BLOCK_SIZE), dummy_data);
    }

    return count;
}

int disk_cache_get_metrics(DiskCacheBuffer *cache, DiskCacheMetrics *metrics)
{
    if (!cache || !metrics)
        return -1;

    metrics->cache_id = cache->cache_manager_id;
    metrics->total_buffer_size = cache->total_buffer_size;
    metrics->allocated_size = cache->allocated_buffer_size;

    if (cache->total_hits + cache->total_misses > 0)
    {
        metrics->hit_ratio = (float)cache->total_hits / (cache->total_hits + cache->total_misses);
    }
    else
    {
        metrics->hit_ratio = 0.0f;
    }

    metrics->total_reads = cache->total_reads;
    metrics->total_writes = cache->total_writes;

    uint32_t dirty_count = 0;
    for (uint32_t i = 0; i < cache->pool_count; i++)
    {
        dirty_count += cache->pools[i].dirty_count;
    }
    metrics->dirty_blocks = dirty_count;

    return 0;
}
