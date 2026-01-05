#include "mm_slub.h"

int mm_slub_init(void)
{
    return 0;
}

int mm_slub_create_cache(const char *name, uint32_t object_size, mm_slub_cache_t *cache)
{
    if (!name || !cache)
    {
        return -1;
    }
    return 0;
}

int mm_slub_destroy_cache(uint32_t cache_id)
{
    return 0;
}

int mm_slub_alloc(uint32_t cache_id, uint64_t *address)
{
    if (!address)
    {
        return -1;
    }
    return 0;
}

int mm_slub_free(uint32_t cache_id, uint64_t address)
{
    return 0;
}

int mm_slub_shrink_cache(uint32_t cache_id)
{
    return 0;
}
