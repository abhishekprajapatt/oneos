#include "var_cache_apt.h"

int var_cache_apt_init(void)
{
    return 0;
}

int var_cache_apt_list(var_cache_apt_entry_t *entries, uint32_t *count)
{
    if (!entries || !count)
    {
        return -1;
    }
    return 0;
}

int var_cache_apt_get(const char *package_name, var_cache_apt_entry_t *entry)
{
    if (!package_name || !entry)
    {
        return -1;
    }
    return 0;
}

int var_cache_apt_store(const char *package_name, const char *version, const void *data, uint64_t size)
{
    if (!package_name || !version || !data)
    {
        return -1;
    }
    return 0;
}

int var_cache_apt_evict(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int var_cache_apt_clear(void)
{
    return 0;
}

int var_cache_apt_get_state(var_cache_apt_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
