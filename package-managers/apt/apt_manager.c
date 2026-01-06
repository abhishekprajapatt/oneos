#include "apt_manager.h"

int apt_manager_init(void)
{
    return 0;
}

int apt_manager_update_repos(void)
{
    return 0;
}

int apt_manager_search_package(const char *package_name, apt_package_t *pkg)
{
    if (!package_name || !pkg)
    {
        return -1;
    }
    return 0;
}

int apt_manager_install_package(const char *package_name, const char *version)
{
    if (!package_name || !version)
    {
        return -1;
    }
    return 0;
}

int apt_manager_remove_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int apt_manager_upgrade_all(void)
{
    return 0;
}

int apt_manager_get_installed(apt_package_t **packages, uint32_t *count)
{
    if (!packages || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int apt_manager_add_ppa(const char *ppa_url)
{
    if (!ppa_url)
    {
        return -1;
    }
    return 0;
}
