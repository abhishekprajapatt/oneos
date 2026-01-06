#include "rpm_manager.h"

int rpm_manager_init(void)
{
    return 0;
}

int rpm_manager_install_rpm(const char *rpm_file_path)
{
    if (!rpm_file_path)
    {
        return -1;
    }
    return 0;
}

int rpm_manager_remove_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int rpm_manager_query_package(const char *package_name, rpm_package_t *pkg)
{
    if (!package_name || !pkg)
    {
        return -1;
    }
    return 0;
}

int rpm_manager_upgrade_package(const char *rpm_file_path)
{
    if (!rpm_file_path)
    {
        return -1;
    }
    return 0;
}

int rpm_manager_check_dependencies(const char *rpm_file_path)
{
    if (!rpm_file_path)
    {
        return -1;
    }
    return 0;
}

int rpm_manager_get_installed(rpm_package_t **packages, uint32_t *count)
{
    if (!packages || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}
