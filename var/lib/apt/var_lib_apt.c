#include "var_lib_apt.h"

int var_lib_apt_init(void)
{
    return 0;
}

int var_lib_apt_list(var_lib_apt_entry_t *packages, uint32_t *count)
{
    if (!packages || !count)
    {
        return -1;
    }
    return 0;
}

int var_lib_apt_get_package(const char *package_name, var_lib_apt_entry_t *package)
{
    if (!package_name || !package)
    {
        return -1;
    }
    return 0;
}

int var_lib_apt_update_package(const char *package_name, const char *status)
{
    if (!package_name || !status)
    {
        return -1;
    }
    return 0;
}

int var_lib_apt_remove_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int var_lib_apt_get_state(var_lib_apt_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
