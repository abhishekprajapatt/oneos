#include "memory_optimization.h"
#include <stdlib.h>
#include <string.h>

MemoryOptimizer *mem_opt_init(uint64_t total_mem)
{
    MemoryOptimizer *optimizer = (MemoryOptimizer *)malloc(sizeof(MemoryOptimizer));
    if (!optimizer)
        return NULL;

    memset(optimizer, 0, sizeof(MemoryOptimizer));
    optimizer->total_memory = total_mem;
    optimizer->used_memory = 0;
    optimizer->compressed_memory = 0;
    optimizer->deduplicated_memory = 0;
    optimizer->region_count = 0;
    optimizer->compression_block_count = 0;
    optimizer->dedup_count = 0;
    optimizer->cache_count = 0;
    optimizer->overall_compression_ratio = 1.0f;
    optimizer->dedup_savings_percent = 0.0f;
    optimizer->default_cache_policy = CACHE_POLICY_LRU;

    return optimizer;
}

void mem_opt_cleanup(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return;

    for (uint32_t i = 0; i < optimizer->compression_block_count; i++)
    {
        if (optimizer->compression_blocks[i].original_data)
        {
            free(optimizer->compression_blocks[i].original_data);
        }
        if (optimizer->compression_blocks[i].compressed_data)
        {
            free(optimizer->compression_blocks[i].compressed_data);
        }
    }

    memset(optimizer, 0, sizeof(MemoryOptimizer));
    free(optimizer);
}

uint32_t mem_opt_register_region(MemoryOptimizer *optimizer, uint64_t address,
                                 uint32_t size, CompressionType compression)
{
    if (!optimizer || optimizer->region_count >= MAX_MEMORY_REGIONS)
        return 0;
    if (size == 0)
        return 0;

    MemoryRegion *region = &optimizer->regions[optimizer->region_count];
    region->region_id = optimizer->region_count + 1;
    region->address = address;
    region->size = size;
    region->compression = compression;
    region->compressed_size = size;
    region->compression_ratio = 1.0f;
    region->access_count = 0;
    region->last_accessed = time(NULL);
    region->is_compressed = false;
    region->is_swapped = false;

    optimizer->used_memory += size;
    optimizer->region_count++;

    return region->region_id;
}

int mem_opt_unregister_region(MemoryOptimizer *optimizer, uint32_t region_id)
{
    if (!optimizer)
        return -1;

    for (uint32_t i = 0; i < optimizer->region_count; i++)
    {
        if (optimizer->regions[i].region_id == region_id)
        {
            optimizer->used_memory -= optimizer->regions[i].size;
            memmove(&optimizer->regions[i], &optimizer->regions[i + 1],
                    (optimizer->region_count - i - 1) * sizeof(MemoryRegion));
            optimizer->region_count--;
            return 0;
        }
    }
    return -1;
}

MemoryRegion *mem_opt_get_region(MemoryOptimizer *optimizer, uint32_t region_id)
{
    if (!optimizer)
        return NULL;

    for (uint32_t i = 0; i < optimizer->region_count; i++)
    {
        if (optimizer->regions[i].region_id == region_id)
        {
            optimizer->regions[i].access_count++;
            optimizer->regions[i].last_accessed = time(NULL);
            return &optimizer->regions[i];
        }
    }
    return NULL;
}

int mem_opt_compress_region(MemoryOptimizer *optimizer, uint32_t region_id)
{
    if (!optimizer)
        return -1;

    MemoryRegion *region = mem_opt_get_region(optimizer, region_id);
    if (!region || region->compression == COMPRESS_NONE)
        return -1;

    uint32_t compressed_size = (region->size * 70) / 100;
    region->compressed_size = compressed_size;
    region->compression_ratio = (float)region->size / compressed_size;
    region->is_compressed = true;

    optimizer->compressed_memory += (region->size - compressed_size);
    optimizer->overall_compression_ratio =
        (float)optimizer->used_memory / (optimizer->used_memory - optimizer->compressed_memory + 1);

    return 0;
}

int mem_opt_decompress_region(MemoryOptimizer *optimizer, uint32_t region_id)
{
    if (!optimizer)
        return -1;

    MemoryRegion *region = mem_opt_get_region(optimizer, region_id);
    if (!region || !region->is_compressed)
        return -1;

    optimizer->compressed_memory -= (region->size - region->compressed_size);
    region->is_compressed = false;
    region->compressed_size = region->size;
    region->compression_ratio = 1.0f;

    return 0;
}

int mem_opt_compress_all(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return -1;

    for (uint32_t i = 0; i < optimizer->region_count; i++)
    {
        mem_opt_compress_region(optimizer, optimizer->regions[i].region_id);
    }
    return 0;
}

uint32_t mem_opt_add_dedup_entry(MemoryOptimizer *optimizer, const uint8_t *content_hash,
                                 uint32_t size)
{
    if (!optimizer || optimizer->dedup_count >= MAX_DEDUP_ENTRIES || !content_hash)
        return 0;

    for (uint32_t i = 0; i < optimizer->dedup_count; i++)
    {
        if (memcmp(optimizer->dedup_table[i].content_hash, content_hash, 32) == 0)
        {
            optimizer->dedup_table[i].reference_count++;
            optimizer->dedup_table[i].shared_refs++;
            return optimizer->dedup_table[i].dedup_id;
        }
    }

    DedupEntry *entry = &optimizer->dedup_table[optimizer->dedup_count];
    entry->dedup_id = optimizer->dedup_count + 1;
    memcpy(entry->content_hash, content_hash, 32);
    entry->reference_count = 1;
    entry->size = size;
    entry->shared_refs = 0;
    entry->created_time = time(NULL);

    optimizer->deduplicated_memory += size;
    optimizer->dedup_count++;

    return entry->dedup_id;
}

int mem_opt_remove_dedup_entry(MemoryOptimizer *optimizer, uint32_t dedup_id)
{
    if (!optimizer)
        return -1;

    for (uint32_t i = 0; i < optimizer->dedup_count; i++)
    {
        if (optimizer->dedup_table[i].dedup_id == dedup_id)
        {
            optimizer->deduplicated_memory -= optimizer->dedup_table[i].size;
            memmove(&optimizer->dedup_table[i], &optimizer->dedup_table[i + 1],
                    (optimizer->dedup_count - i - 1) * sizeof(DedupEntry));
            optimizer->dedup_count--;
            return 0;
        }
    }
    return -1;
}

int mem_opt_dedup_scan(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return -1;

    uint32_t dedup_count = 0;
    for (uint32_t i = 0; i < optimizer->dedup_count; i++)
    {
        if (optimizer->dedup_table[i].reference_count > 1)
        {
            dedup_count++;
        }
    }

    if (dedup_count > 0)
    {
        optimizer->dedup_savings_percent =
            (float)(optimizer->deduplicated_memory * dedup_count) /
            (optimizer->used_memory + 1) * 100.0f;
    }

    return dedup_count;
}

DedupEntry *mem_opt_find_dedup(MemoryOptimizer *optimizer, const uint8_t *content_hash)
{
    if (!optimizer || !content_hash)
        return NULL;

    for (uint32_t i = 0; i < optimizer->dedup_count; i++)
    {
        if (memcmp(optimizer->dedup_table[i].content_hash, content_hash, 32) == 0)
        {
            return &optimizer->dedup_table[i];
        }
    }
    return NULL;
}

uint32_t mem_opt_cache_allocate(MemoryOptimizer *optimizer, uint64_t address, uint32_t size)
{
    if (!optimizer || optimizer->cache_count >= MAX_CACHE_ENTRIES)
        return 0;

    CacheEntry *entry = &optimizer->cache[optimizer->cache_count];
    entry->cache_id = optimizer->cache_count + 1;
    entry->address = address;
    entry->size = size;
    entry->policy = optimizer->default_cache_policy;
    entry->access_count = 0;
    entry->created_time = time(NULL);
    entry->last_accessed = time(NULL);
    entry->is_dirty = false;

    optimizer->cache_count++;
    return entry->cache_id;
}

int mem_opt_cache_deallocate(MemoryOptimizer *optimizer, uint32_t cache_id)
{
    if (!optimizer)
        return -1;

    for (uint32_t i = 0; i < optimizer->cache_count; i++)
    {
        if (optimizer->cache[i].cache_id == cache_id)
        {
            memmove(&optimizer->cache[i], &optimizer->cache[i + 1],
                    (optimizer->cache_count - i - 1) * sizeof(CacheEntry));
            optimizer->cache_count--;
            return 0;
        }
    }
    return -1;
}

int mem_opt_cache_evict(MemoryOptimizer *optimizer, uint32_t count)
{
    if (!optimizer || count > optimizer->cache_count)
        return -1;

    for (uint32_t i = 0; i < count; i++)
    {
        if (optimizer->cache_count > 0)
        {
            uint32_t lru_idx = 0;
            for (uint32_t j = 1; j < optimizer->cache_count; j++)
            {
                if (optimizer->cache[j].last_accessed < optimizer->cache[lru_idx].last_accessed)
                {
                    lru_idx = j;
                }
            }
            memmove(&optimizer->cache[lru_idx], &optimizer->cache[lru_idx + 1],
                    (optimizer->cache_count - lru_idx - 1) * sizeof(CacheEntry));
            optimizer->cache_count--;
        }
    }
    return 0;
}

int mem_opt_cache_set_policy(MemoryOptimizer *optimizer, CachePolicy policy)
{
    if (!optimizer)
        return -1;
    optimizer->default_cache_policy = policy;
    return 0;
}

int mem_opt_swap_out(MemoryOptimizer *optimizer, uint32_t region_id)
{
    if (!optimizer)
        return -1;

    MemoryRegion *region = mem_opt_get_region(optimizer, region_id);
    if (!region)
        return -1;

    region->is_swapped = true;
    optimizer->used_memory -= region->size;
    return 0;
}

int mem_opt_swap_in(MemoryOptimizer *optimizer, uint32_t region_id)
{
    if (!optimizer)
        return -1;

    MemoryRegion *region = mem_opt_get_region(optimizer, region_id);
    if (!region)
        return -1;

    region->is_swapped = false;
    optimizer->used_memory += region->size;
    return 0;
}

uint64_t mem_opt_get_used_memory(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return 0;
    return optimizer->used_memory;
}

uint64_t mem_opt_get_available_memory(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return 0;
    return optimizer->total_memory - optimizer->used_memory;
}

uint64_t mem_opt_get_compressed_memory(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return 0;
    return optimizer->compressed_memory;
}

uint64_t mem_opt_get_deduplicated_memory(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return 0;
    return optimizer->deduplicated_memory;
}

float mem_opt_get_compression_ratio(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return 1.0f;
    return optimizer->overall_compression_ratio;
}

float mem_opt_get_dedup_savings(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return 0.0f;
    return optimizer->dedup_savings_percent;
}

float mem_opt_get_memory_efficiency(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return 0.0f;
    return (float)optimizer->used_memory / optimizer->total_memory * 100.0f;
}

int mem_opt_defragment(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return -1;
    return 0;
}

int mem_opt_garbage_collect(MemoryOptimizer *optimizer)
{
    if (!optimizer)
        return -1;

    for (uint32_t i = 0; i < optimizer->dedup_count; i++)
    {
        if (optimizer->dedup_table[i].reference_count == 0)
        {
            mem_opt_remove_dedup_entry(optimizer, optimizer->dedup_table[i].dedup_id);
        }
    }
    return 0;
}
