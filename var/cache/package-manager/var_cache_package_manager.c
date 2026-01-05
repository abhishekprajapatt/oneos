#include "var_cache_package_manager.h"

int var_cache_package_manager_init(void)
{
    return 0;
}

int var_cache_package_manager_list(var_cache_package_manager_entry_t *entries, uint32_t *count)
{
    if (!entries || !count)
    {
        return -1;
    }
    return 0;
}

int var_cache_package_manager_get(const char *package_name, var_cache_package_manager_entry_t *entry)
{
    if (!package_name || !entry)
    {
        return -1;
    }
    return 0;
}

int var_cache_package_manager_cache_package(const char *package_name, const char *version, const char *manager)
{
    if (!package_name || !version || !manager)
    {
        return -1;
    }
    return 0;
}

int var_cache_package_manager_evict(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int var_cache_package_manager_clear(void)
{
    return 0;
}

int var_cache_package_manager_get_state(var_cache_package_manager_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
