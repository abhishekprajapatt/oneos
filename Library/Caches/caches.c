#include "caches.h"

int caches_init(void)
{
    return 0;
}

int caches_get_cache_dir(const char *app_name, char *path, uint32_t max_len)
{
    if (!app_name || !path || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int caches_clear_cache(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}

int caches_clear_all_caches(void)
{
    return 0;
}

int caches_get_cache_size(const char *app_name, uint32_t *size)
{
    if (!app_name || !size)
    {
        return -1;
    }
    *size = 0;
    return 0;
}

int caches_set_cache_limit(uint32_t limit)
{
    if (limit == 0)
    {
        return -1;
    }
    return 0;
}
