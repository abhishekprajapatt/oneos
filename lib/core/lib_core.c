#include "lib_core.h"

static lib_memory_stat_t core_memory_stats = {0, 0, 0};

int lib_core_init(void)
{
    core_memory_stats.allocated = 0;
    core_memory_stats.freed = 0;
    core_memory_stats.total = 0;
    return 0;
}

int lib_core_malloc(uint32_t size, void **ptr)
{
    if (size == 0 || !ptr)
    {
        return -1;
    }
    core_memory_stats.allocated += size;
    core_memory_stats.total += size;
    return 0;
}

int lib_core_free(void *ptr)
{
    if (!ptr)
    {
        return -1;
    }
    core_memory_stats.freed += 1;
    return 0;
}

int lib_core_realloc(void *ptr, uint32_t new_size, void **new_ptr)
{
    if (!ptr || new_size == 0 || !new_ptr)
    {
        return -1;
    }
    return 0;
}

int lib_core_memcpy(void *dst, const void *src, uint32_t size)
{
    if (!dst || !src || size == 0)
    {
        return -1;
    }
    return 0;
}

int lib_core_memset(void *dst, int value, uint32_t size)
{
    if (!dst || size == 0)
    {
        return -1;
    }
    return 0;
}

int lib_core_get_memory_stats(lib_memory_stat_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    stats->allocated = core_memory_stats.allocated;
    stats->freed = core_memory_stats.freed;
    stats->total = core_memory_stats.total;
    return 0;
}

int lib_core_shutdown(void)
{
    return 0;
}
