#ifndef DISK_CACHE_BUFFER_H
#define DISK_CACHE_BUFFER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_BUFFER_BLOCKS 65536
#define MAX_BUFFER_POOLS 16
#define BUFFER_BLOCK_SIZE 8192

typedef enum
{
    BUFFER_FREE,
    BUFFER_ALLOCATED,
    BUFFER_DIRTY,
    BUFFER_LOCKED
} BufferBlockState;

typedef struct
{
    uint32_t block_id;
    void *block_ptr;
    uint64_t disk_location;
    BufferBlockState state;

    uint32_t reference_count;
    uint8_t is_dirty;
    uint8_t is_pinned;

    uint64_t allocation_time;
    uint64_t last_access_time;
    uint32_t access_count;
} BufferBlock;

typedef struct
{
    uint32_t pool_id;
    uint32_t block_count;
    BufferBlock *blocks;
    uint64_t total_pool_size;
    uint64_t allocated_size;

    uint32_t dirty_count;
    uint32_t hit_count;
    uint32_t miss_count;
} BufferPool;

typedef struct
{
    uint32_t cache_manager_id;
    uint32_t pool_count;
    BufferPool pools[MAX_BUFFER_POOLS];

    uint64_t total_buffer_size;
    uint64_t allocated_buffer_size;

    uint64_t total_reads;
    uint64_t total_writes;
    uint64_t total_flushes;

    uint32_t total_hits;
    uint32_t total_misses;

    float hit_ratio;
    float utilization_ratio;
} DiskCacheBuffer;

DiskCacheBuffer *disk_cache_init(uint64_t buffer_size, uint32_t pool_count);
int disk_cache_destroy(DiskCacheBuffer *cache);

BufferBlock *disk_cache_allocate_block(DiskCacheBuffer *cache, uint32_t pool_id);
int disk_cache_deallocate_block(DiskCacheBuffer *cache, uint32_t pool_id, uint32_t block_id);

BufferBlock *disk_cache_read_block(DiskCacheBuffer *cache, uint64_t disk_location);
int disk_cache_write_block(DiskCacheBuffer *cache, uint64_t disk_location, const uint8_t *data);

int disk_cache_flush_blocks(DiskCacheBuffer *cache, uint32_t pool_id);
int disk_cache_flush_all(DiskCacheBuffer *cache);

int disk_cache_lock_block(DiskCacheBuffer *cache, uint32_t block_id);
int disk_cache_unlock_block(DiskCacheBuffer *cache, uint32_t block_id);

int disk_cache_prefetch_blocks(DiskCacheBuffer *cache, uint64_t start_location, uint32_t count);

typedef struct
{
    uint32_t cache_id;
    uint64_t total_buffer_size;
    uint64_t allocated_size;
    float hit_ratio;
    uint64_t total_reads;
    uint64_t total_writes;
    uint32_t dirty_blocks;
} DiskCacheMetrics;

int disk_cache_get_metrics(DiskCacheBuffer *cache, DiskCacheMetrics *metrics);

#endif
