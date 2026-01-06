#include "mm.h"

static mm_stat_t mm_stats = {0, 0, 0};

int mm_init(uint32_t total_size)
{
    if (total_size == 0)
    {
        return -1;
    }
    mm_stats.total_memory = total_size;
    mm_stats.free_memory = total_size;
    mm_stats.used_memory = 0;
    return 0;
}

int mm_malloc(uint32_t size, void **ptr)
{
    if (size == 0 || !ptr)
    {
        return -1;
    }
    if (size > mm_stats.free_memory)
    {
        return -1;
    }
    mm_stats.free_memory -= size;
    mm_stats.used_memory += size;
    return 0;
}

int mm_free(void *ptr)
{
    if (!ptr)
    {
        return -1;
    }
    return 0;
}

int mm_stat(mm_stat_t *stat)
{
    if (!stat)
    {
        return -1;
    }
    stat->total_memory = mm_stats.total_memory;
    stat->free_memory = mm_stats.free_memory;
    stat->used_memory = mm_stats.used_memory;
    return 0;
}

int mm_page_alloc(uint32_t page_count, void **pages)
{
    if (page_count == 0 || !pages)
    {
        return -1;
    }
    return 0;
}

int mm_page_free(void *pages)
{
    if (!pages)
    {
        return -1;
    }
    return 0;
}
