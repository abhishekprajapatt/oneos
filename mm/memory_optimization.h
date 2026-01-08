#ifndef MEMORY_OPTIMIZATION_H
#define MEMORY_OPTIMIZATION_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_MEMORY_REGIONS 2048
#define MAX_COMPRESSION_BLOCKS 4096
#define MAX_DEDUP_ENTRIES 8192
#define MAX_CACHE_ENTRIES 2048

typedef enum
{
    COMPRESS_NONE = 0,
    COMPRESS_LZ4 = 1,
    COMPRESS_ZSTD = 2,
    COMPRESS_GZIP = 3
} CompressionType;

typedef enum
{
    CACHE_POLICY_LRU = 0,
    CACHE_POLICY_LFU = 1,
    CACHE_POLICY_FIFO = 2,
    CACHE_POLICY_ARC = 3
} CachePolicy;

typedef struct
{
    uint32_t region_id;
    uint64_t address;
    uint32_t size;
    CompressionType compression;
    uint32_t compressed_size;
    float compression_ratio;
    uint32_t access_count;
    time_t last_accessed;
    bool is_compressed;
    bool is_swapped;
} MemoryRegion;

typedef struct
{
    uint32_t block_id;
    uint8_t *original_data;
    uint8_t *compressed_data;
    uint32_t original_size;
    uint32_t compressed_size;
    CompressionType method;
    uint32_t compression_time_us;
    float efficiency;
} CompressionBlock;

typedef struct
{
    uint32_t dedup_id;
    uint8_t content_hash[32];
    uint64_t reference_count;
    uint32_t size;
    uint32_t shared_refs;
    time_t created_time;
} DedupEntry;

typedef struct
{
    uint32_t cache_id;
    uint64_t address;
    uint32_t size;
    CachePolicy policy;
    uint32_t access_count;
    time_t last_accessed;
    time_t created_time;
    bool is_dirty;
} CacheEntry;

typedef struct
{
    MemoryRegion regions[MAX_MEMORY_REGIONS];
    uint32_t region_count;
    CompressionBlock compression_blocks[MAX_COMPRESSION_BLOCKS];
    uint32_t compression_block_count;
    DedupEntry dedup_table[MAX_DEDUP_ENTRIES];
    uint32_t dedup_count;
    CacheEntry cache[MAX_CACHE_ENTRIES];
    uint32_t cache_count;
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t compressed_memory;
    uint64_t deduplicated_memory;
    float overall_compression_ratio;
    float dedup_savings_percent;
    CachePolicy default_cache_policy;
} MemoryOptimizer;

MemoryOptimizer *mem_opt_init(uint64_t total_mem);
void mem_opt_cleanup(MemoryOptimizer *optimizer);

uint32_t mem_opt_register_region(MemoryOptimizer *optimizer, uint64_t address,
                                 uint32_t size, CompressionType compression);
int mem_opt_unregister_region(MemoryOptimizer *optimizer, uint32_t region_id);
MemoryRegion *mem_opt_get_region(MemoryOptimizer *optimizer, uint32_t region_id);

int mem_opt_compress_region(MemoryOptimizer *optimizer, uint32_t region_id);
int mem_opt_decompress_region(MemoryOptimizer *optimizer, uint32_t region_id);
int mem_opt_compress_all(MemoryOptimizer *optimizer);

uint32_t mem_opt_add_dedup_entry(MemoryOptimizer *optimizer, const uint8_t *content_hash,
                                 uint32_t size);
int mem_opt_remove_dedup_entry(MemoryOptimizer *optimizer, uint32_t dedup_id);
int mem_opt_dedup_scan(MemoryOptimizer *optimizer);
DedupEntry *mem_opt_find_dedup(MemoryOptimizer *optimizer, const uint8_t *content_hash);

uint32_t mem_opt_cache_allocate(MemoryOptimizer *optimizer, uint64_t address, uint32_t size);
int mem_opt_cache_deallocate(MemoryOptimizer *optimizer, uint32_t cache_id);
int mem_opt_cache_evict(MemoryOptimizer *optimizer, uint32_t count);
int mem_opt_cache_set_policy(MemoryOptimizer *optimizer, CachePolicy policy);

int mem_opt_swap_out(MemoryOptimizer *optimizer, uint32_t region_id);
int mem_opt_swap_in(MemoryOptimizer *optimizer, uint32_t region_id);

uint64_t mem_opt_get_used_memory(MemoryOptimizer *optimizer);
uint64_t mem_opt_get_available_memory(MemoryOptimizer *optimizer);
uint64_t mem_opt_get_compressed_memory(MemoryOptimizer *optimizer);
uint64_t mem_opt_get_deduplicated_memory(MemoryOptimizer *optimizer);

float mem_opt_get_compression_ratio(MemoryOptimizer *optimizer);
float mem_opt_get_dedup_savings(MemoryOptimizer *optimizer);
float mem_opt_get_memory_efficiency(MemoryOptimizer *optimizer);

int mem_opt_defragment(MemoryOptimizer *optimizer);
int mem_opt_garbage_collect(MemoryOptimizer *optimizer);

#endif
