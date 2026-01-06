#include "installer.h"

int installer_init(void)
{
    return 0;
}

int installer_install_package(const char *package_path)
{
    if (!package_path)
    {
        return -1;
    }
    return 0;
}

int installer_uninstall_package(const char *package_name)
{
    if (!package_name)
    {
        return -1;
    }
    return 0;
}

int installer_verify_integrity(const char *package_path)
{
    if (!package_path)
    {
        return -1;
    }
    return 0;
}

int installer_get_progress(uint32_t *progress)
{
    if (!progress)
    {
        return -1;
    }
    *progress = 0;
    return 0;
}

int installer_cancel_install(void)
{
    return 0;
}
