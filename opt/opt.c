#include "opt.h"

int opt_init(void)
{
    return 0;
}

int opt_install_package(const char *package)
{
    if (!package)
    {
        return -1;
    }
    return 0;
}

int opt_uninstall_package(const char *package)
{
    if (!package)
    {
        return -1;
    }
    return 0;
}

int opt_get_installed(char **packages, uint32_t max_packages)
{
    if (!packages || max_packages == 0)
    {
        return -1;
    }
    return 0;
}

int opt_check_integrity(const char *package)
{
    if (!package)
    {
        return -1;
    }
    return 0;
}
