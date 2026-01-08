#ifndef MEMORY_POOL_ALLOCATOR_H
#define MEMORY_POOL_ALLOCATOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_MEMORY_POOLS 64
#define MAX_POOL_SIZE (1024 * 1024 * 1024)
#define MAX_BLOCK_SIZES 32

typedef enum
{
    POOL_FIXED_SIZE,
    POOL_VARIABLE_SIZE,
    POOL_SLAB
} PoolType;

typedef struct
{
    uint32_t block_id;
    void *block_ptr;
    uint64_t block_size;
    uint8_t is_allocated;
    uint32_t allocations;
    uint64_t allocation_time;
} MemoryBlock;

typedef struct
{
    uint32_t pool_id;
    PoolType pool_type;

    uint64_t pool_size;
    void *pool_base;

    uint32_t block_count;
    MemoryBlock blocks[4096];

    uint64_t total_allocated;
    uint64_t total_free;
    uint64_t peak_usage;

    uint32_t allocation_count;
    uint32_t deallocation_count;

    float fragmentation_ratio;
} MemoryPool;

typedef struct
{
    uint32_t allocator_id;

    uint32_t pool_count;
    MemoryPool pools[MAX_MEMORY_POOLS];

    uint64_t total_memory_managed;
    uint64_t total_allocated;
    uint64_t total_free;

    uint32_t total_allocations;
    uint32_t total_deallocations;

    float average_fragmentation;
} MemoryAllocator;

MemoryAllocator *allocator_init(uint64_t total_memory);
int allocator_destroy(MemoryAllocator *allocator);

MemoryPool *allocator_create_pool(MemoryAllocator *allocator, PoolType type, uint64_t pool_size);
int allocator_destroy_pool(MemoryAllocator *allocator, uint32_t pool_id);

void *allocator_allocate(MemoryAllocator *allocator, uint32_t pool_id, uint64_t size);
int allocator_deallocate(MemoryAllocator *allocator, uint32_t pool_id, void *ptr);

int allocator_resize_block(MemoryAllocator *allocator, uint32_t pool_id, void *ptr, uint64_t new_size);

int allocator_defragment_pool(MemoryAllocator *allocator, uint32_t pool_id);
int allocator_compact_pool(MemoryAllocator *allocator, uint32_t pool_id);

int allocator_enable_statistics(MemoryAllocator *allocator);
int allocator_get_pool_statistics(MemoryAllocator *allocator, uint32_t pool_id);

typedef struct
{
    uint32_t allocator_id;
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    float utilization_percent;
    float fragmentation_ratio;
    uint32_t total_allocations;
} AllocatorMetrics;

int allocator_get_metrics(MemoryAllocator *allocator, AllocatorMetrics *metrics);

#endif
