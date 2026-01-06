#include "programdata.h"

int programdata_init(void)
{
    return 0;
}

int programdata_create_app_folder(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}

int programdata_write_config(const char *app_name, const char *config_data)
{
    if (!app_name || !config_data)
    {
        return -1;
    }
    return 0;
}

int programdata_read_config(const char *app_name, char *config_data, uint32_t max_len)
{
    if (!app_name || !config_data || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int programdata_delete_app_data(const char *app_name)
{
    if (!app_name)
    {
        return -1;
    }
    return 0;
}

int programdata_get_app_size(const char *app_name, uint64_t *size)
{
    if (!app_name || !size)
    {
        return -1;
    }
    *size = 0;
    return 0;
}

int programdata_list_apps(char **apps, uint32_t max_apps)
{
    if (!apps || max_apps == 0)
    {
        return -1;
    }
    return 0;
}
