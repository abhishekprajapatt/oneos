#include "apt_cache.h"

int apt_cache_init(void)
{
    return 0;
}

int apt_cache_update(void)
{
    return 0;
}

int apt_cache_search(const char *query)
{
    if (!query)
    {
        return -1;
    }
    return 0;
}

int apt_cache_show_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int apt_cache_depends(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int apt_cache_clean(void)
{
    return 0;
}

int apt_cache_autoclean(void)
{
    return 0;
}
