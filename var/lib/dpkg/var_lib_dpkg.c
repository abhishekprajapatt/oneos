#include "var_lib_dpkg.h"

int var_lib_dpkg_init(void)
{
    return 0;
}

int var_lib_dpkg_list(var_lib_dpkg_entry_t *packages, uint32_t *count)
{
    if (!packages || !count)
    {
        return -1;
    }
    return 0;
}

int var_lib_dpkg_get_package(const char *package_name, var_lib_dpkg_entry_t *package)
{
    if (!package_name || !package)
    {
        return -1;
    }
    return 0;
}

int var_lib_dpkg_mark_configured(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int var_lib_dpkg_mark_unconfigured(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int var_lib_dpkg_get_state(var_lib_dpkg_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
