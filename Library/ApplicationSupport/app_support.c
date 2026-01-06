#include "app_support.h"

int app_support_init(void)
{
    return 0;
}

int app_support_get_path(const char *app_name, char *path, uint32_t max_len)
{
    if (!app_name || !path || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int app_support_create_directory(const char *app_name, const char *dir)
{
    if (!app_name || !dir)
    {
        return -1;
    }
    return 0;
}

int app_support_save_data(const char *app_name, const char *data_name, const void *data, uint32_t size)
{
    if (!app_name || !data_name || !data || size == 0)
    {
        return -1;
    }
    return 0;
}

int app_support_load_data(const char *app_name, const char *data_name, void *buffer, uint32_t max_size)
{
    if (!app_name || !data_name || !buffer || max_size == 0)
    {
        return -1;
    }
    return 0;
}

int app_support_list_apps(char **apps, uint32_t max_apps)
{
    if (!apps || max_apps == 0)
    {
        return -1;
    }
    return 0;
}
