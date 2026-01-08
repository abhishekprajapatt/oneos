#include "page_cache_manager.h"
#include <stdlib.h>
#include <string.h>

static PageCacheManager *global_cache_manager = NULL;
static uint32_t next_cache_page_id = 1;

PageCacheManager *cache_manager_init(uint64_t cache_size, EvictionPolicy policy)
{
    PageCacheManager *manager = (PageCacheManager *)malloc(sizeof(PageCacheManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(PageCacheManager));
    manager->cache_manager_id = 1;
    manager->policy = policy;
    manager->zone_count = 4;
    manager->total_cache_pages = cache_size / PAGE_CACHE_SIZE;
    manager->total_cache_size = cache_size;
    manager->cache_hits = 0;
    manager->cache_misses = 0;
    manager->pages_evicted = 0;
    manager->pages_writeback = 0;
    manager->overall_hit_ratio = 0.0f;
    manager->dirty_page_count = 0;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        manager->zones[i].zone_id = i;
        manager->zones[i].page_count = manager->total_cache_pages / manager->zone_count;
        manager->zones[i].pages = (CachePage *)malloc(sizeof(CachePage) * manager->zones[i].page_count);
        if (!manager->zones[i].pages)
        {
            free(manager);
            return NULL;
        }
        memset(manager->zones[i].pages, 0, sizeof(CachePage) * manager->zones[i].page_count);
        manager->zones[i].total_size = manager->zones[i].page_count * PAGE_CACHE_SIZE;
        manager->zones[i].used_size = 0;
        manager->zones[i].hit_ratio = 0.0f;
    }

    global_cache_manager = manager;
    return manager;
}

int cache_manager_destroy(PageCacheManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        if (manager->zones[i].pages)
        {
            for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
            {
                if (manager->zones[i].pages[j].page_data)
                    free(manager->zones[i].pages[j].page_data);
            }
            free(manager->zones[i].pages);
        }
    }

    free(manager);
    global_cache_manager = NULL;
    return 0;
}

CachePage *cache_read_page(PageCacheManager *manager, uint64_t disk_offset)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
        {
            CachePage *page = &manager->zones[i].pages[j];
            if (page->disk_offset == disk_offset && page->page_data != NULL)
            {
                page->access_count++;
                page->last_access_time = 0;
                manager->cache_hits++;
                return page;
            }
        }
    }

    manager->cache_misses++;
    return NULL;
}

int cache_write_page(PageCacheManager *manager, uint64_t disk_offset, const uint8_t *data)
{
    if (!manager || !data)
        return -1;

    CacheZone *best_zone = NULL;
    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        if (manager->zones[i].used_size < manager->zones[i].total_size)
        {
            if (!best_zone || manager->zones[i].used_size < best_zone->used_size)
            {
                best_zone = &manager->zones[i];
            }
        }
    }

    if (!best_zone)
        return -1;

    uint32_t free_idx = 0;
    for (uint32_t j = 0; j < best_zone->page_count; j++)
    {
        if (best_zone->pages[j].page_data == NULL)
        {
            free_idx = j;
            break;
        }
    }

    CachePage *page = &best_zone->pages[free_idx];
    page->cache_page_id = next_cache_page_id++;
    page->disk_offset = disk_offset;
    page->page_data = (uint8_t *)malloc(PAGE_CACHE_SIZE);
    if (!page->page_data)
        return -1;

    memcpy(page->page_data, data, PAGE_CACHE_SIZE);
    page->state = CACHE_DIRTY;
    page->access_count = 0;
    page->last_access_time = 0;
    page->creation_time = 0;
    page->is_pinned = 0;
    page->is_dirty = 1;
    page->dirty_count = 1;

    best_zone->used_size += PAGE_CACHE_SIZE;
    manager->dirty_page_count++;

    return 0;
}

int cache_evict_page(PageCacheManager *manager, uint32_t page_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
        {
            if (manager->zones[i].pages[j].cache_page_id == page_id && manager->zones[i].pages[j].page_data != NULL)
            {
                CachePage *page = &manager->zones[i].pages[j];

                if (page->is_pinned)
                    return -1;

                free(page->page_data);
                manager->zones[i].used_size -= PAGE_CACHE_SIZE;
                manager->pages_evicted++;

                if (page->is_dirty)
                {
                    manager->dirty_page_count--;
                }

                memset(page, 0, sizeof(CachePage));
                return 0;
            }
        }
    }

    return -1;
}

int cache_evict_lru_page(PageCacheManager *manager)
{
    if (!manager)
        return -1;

    uint64_t oldest_time = UINT64_MAX;
    uint32_t oldest_page_id = 0;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
        {
            CachePage *page = &manager->zones[i].pages[j];
            if (page->page_data != NULL && !page->is_pinned && page->last_access_time < oldest_time)
            {
                oldest_time = page->last_access_time;
                oldest_page_id = page->cache_page_id;
            }
        }
    }

    if (oldest_page_id > 0)
    {
        return cache_evict_page(manager, oldest_page_id);
    }

    return -1;
}

int cache_evict_lfu_page(PageCacheManager *manager)
{
    if (!manager)
        return -1;

    uint32_t min_access_count = UINT32_MAX;
    uint32_t lfu_page_id = 0;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
        {
            CachePage *page = &manager->zones[i].pages[j];
            if (page->page_data != NULL && !page->is_pinned && page->access_count < min_access_count)
            {
                min_access_count = page->access_count;
                lfu_page_id = page->cache_page_id;
            }
        }
    }

    if (lfu_page_id > 0)
    {
        return cache_evict_page(manager, lfu_page_id);
    }

    return -1;
}

int cache_writeback_dirty_pages(PageCacheManager *manager)
{
    if (!manager)
        return -1;

    uint32_t writeback_count = 0;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
        {
            CachePage *page = &manager->zones[i].pages[j];
            if (page->is_dirty && page->page_data != NULL)
            {
                page->state = CACHE_WRITEBACK;
                writeback_count++;
                manager->pages_writeback++;
            }
        }
    }

    return writeback_count;
}

int cache_flush_zone(PageCacheManager *manager, uint32_t zone_id)
{
    if (!manager || zone_id >= manager->zone_count)
        return -1;

    CacheZone *zone = &manager->zones[zone_id];
    for (uint32_t j = 0; j < zone->page_count; j++)
    {
        if (zone->pages[j].page_data != NULL)
        {
            free(zone->pages[j].page_data);
            zone->used_size -= PAGE_CACHE_SIZE;
            memset(&zone->pages[j], 0, sizeof(CachePage));
        }
    }

    return 0;
}

int cache_lock_page(PageCacheManager *manager, uint32_t page_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
        {
            if (manager->zones[i].pages[j].cache_page_id == page_id)
            {
                manager->zones[i].pages[j].is_pinned = 1;
                return 0;
            }
        }
    }

    return -1;
}

int cache_unlock_page(PageCacheManager *manager, uint32_t page_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        for (uint32_t j = 0; j < manager->zones[i].page_count; j++)
        {
            if (manager->zones[i].pages[j].cache_page_id == page_id)
            {
                manager->zones[i].pages[j].is_pinned = 0;
                return 0;
            }
        }
    }

    return -1;
}

int cache_prefetch_pages(PageCacheManager *manager, uint64_t start_offset, uint32_t count)
{
    if (!manager || count == 0)
        return -1;

    for (uint32_t i = 0; i < count; i++)
    {
        uint8_t dummy_data[PAGE_CACHE_SIZE] = {0};
        cache_write_page(manager, start_offset + (i * PAGE_CACHE_SIZE), dummy_data);
    }

    return count;
}

int cache_get_metrics(PageCacheManager *manager, CacheMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->cache_manager_id;
    metrics->total_cache_size = manager->total_cache_size;

    uint64_t used_size = 0;
    for (uint32_t i = 0; i < manager->zone_count; i++)
    {
        used_size += manager->zones[i].used_size;
    }
    metrics->used_cache_size = used_size;

    if (manager->cache_hits + manager->cache_misses > 0)
    {
        metrics->hit_ratio = (float)manager->cache_hits / (manager->cache_hits + manager->cache_misses);
    }
    else
    {
        metrics->hit_ratio = 0.0f;
    }

    metrics->total_hits = manager->cache_hits;
    metrics->total_misses = manager->cache_misses;
    metrics->dirty_pages = manager->dirty_page_count;

    return 0;
}
