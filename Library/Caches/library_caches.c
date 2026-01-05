#include "library_caches.h"

int library_caches_init(void)
{
    return 0;
}

int library_caches_create(library_caches_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int library_caches_delete(uint32_t cache_id)
{
    return 0;
}

int library_caches_clear_app_cache(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}

int library_caches_resize(uint32_t cache_id, uint64_t new_size)
{
    return 0;
}

int library_caches_get_state(library_caches_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
