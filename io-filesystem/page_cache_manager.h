#ifndef PAGE_CACHE_MANAGER_H
#define PAGE_CACHE_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_CACHE_PAGES 131072
#define MAX_DIRTY_PAGES 16384
#define MAX_CACHE_ZONES 8
#define PAGE_CACHE_SIZE 4096

typedef enum
{
    CACHE_CLEAN,
    CACHE_DIRTY,
    CACHE_WRITEBACK,
    CACHE_LOCKED
} CachePageState;

typedef enum
{
    EVICT_LRU,
    EVICT_LFU,
    EVICT_FIFO,
    EVICT_CLOCK,
    EVICT_ARC
} EvictionPolicy;

typedef struct
{
    uint32_t cache_page_id;
    uint64_t disk_offset;
    uint8_t *page_data;
    CachePageState state;

    uint32_t access_count;
    uint64_t last_access_time;
    uint64_t creation_time;

    uint8_t is_pinned;
    uint8_t is_dirty;
    uint32_t dirty_count;
} CachePage;

typedef struct
{
    uint32_t zone_id;
    uint32_t page_count;
    CachePage *pages;
    uint64_t total_size;
    uint64_t used_size;
    float hit_ratio;
} CacheZone;

typedef struct
{
    uint32_t cache_manager_id;
    EvictionPolicy policy;

    uint32_t zone_count;
    CacheZone zones[MAX_CACHE_ZONES];

    uint32_t total_cache_pages;
    uint64_t total_cache_size;

    uint64_t cache_hits;
    uint64_t cache_misses;
    uint64_t pages_evicted;
    uint64_t pages_writeback;

    float overall_hit_ratio;
    uint32_t dirty_page_count;
} PageCacheManager;

PageCacheManager *cache_manager_init(uint64_t cache_size, EvictionPolicy policy);
int cache_manager_destroy(PageCacheManager *manager);

CachePage *cache_read_page(PageCacheManager *manager, uint64_t disk_offset);
int cache_write_page(PageCacheManager *manager, uint64_t disk_offset, const uint8_t *data);

int cache_evict_page(PageCacheManager *manager, uint32_t page_id);
int cache_evict_lru_page(PageCacheManager *manager);
int cache_evict_lfu_page(PageCacheManager *manager);

int cache_writeback_dirty_pages(PageCacheManager *manager);
int cache_flush_zone(PageCacheManager *manager, uint32_t zone_id);

int cache_lock_page(PageCacheManager *manager, uint32_t page_id);
int cache_unlock_page(PageCacheManager *manager, uint32_t page_id);

int cache_prefetch_pages(PageCacheManager *manager, uint64_t start_offset, uint32_t count);

typedef struct
{
    uint32_t manager_id;
    uint64_t total_cache_size;
    uint64_t used_cache_size;
    float hit_ratio;
    uint64_t total_hits;
    uint64_t total_misses;
    uint32_t dirty_pages;
} CacheMetrics;

int cache_get_metrics(PageCacheManager *manager, CacheMetrics *metrics);

#endif
