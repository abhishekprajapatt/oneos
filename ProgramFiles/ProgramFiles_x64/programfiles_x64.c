#include "programfiles_x64.h"

int programfiles_x64_init(void)
{
    return 0;
}

int programfiles_x64_register_app(const char *app_name, const char *app_path)
{
    if (!app_name || !app_path)
    {
        return -1;
    }
    return 0;
}

int programfiles_x64_uninstall_app(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}

int programfiles_x64_list_installed(char **apps, uint32_t max_apps)
{
    if (!apps || max_apps == 0)
    {
        return -1;
    }
    return 0;
}

int programfiles_x64_get_app_info(const char *app_name, char *info, uint32_t max_len)
{
    if (!app_name || !info || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int programfiles_x64_verify_compatibility(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}
