#include "app_compat.h"

int app_compat_init(void)
{
    return 0;
}

int app_compat_enable_mode(const char *app_name, app_compat_mode_t *mode)
{
    if (!app_name || !mode)
    {
        return -1;
    }
    return 0;
}

int app_compat_set_privileges(const char *app_name, uint32_t privilege_level)
{
    if (!app_name || privilege_level == 0)
    {
        return -1;
    }
    return 0;
}

int app_compat_disable_fullscreen_optimization(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}

int app_compat_get_config(const char *app_name, char *config, uint32_t max_len)
{
    if (!app_name || !config || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int app_compat_test_launch(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}
