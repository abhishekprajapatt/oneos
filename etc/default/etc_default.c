#include "etc_default.h"

int etc_default_init(void)
{
    return 0;
}

int etc_default_load_config(const char *filename, etc_default_config_t *config)
{
    if (!filename || !config)
    {
        return -1;
    }
    return 0;
}

int etc_default_save_config(const char *filename, etc_default_config_t *config)
{
    if (!filename || !config)
    {
        return -1;
    }
    return 0;
}

int etc_default_get_param(const char *filename, const char *param_name, char *value, uint32_t size)
{
    if (!filename || !param_name || !value)
    {
        return -1;
    }
    return 0;
}

int etc_default_set_param(const char *filename, const char *param_name, const char *param_value)
{
    if (!filename || !param_name || !param_value)
    {
        return -1;
    }
    return 0;
}

int etc_default_delete_param(const char *filename, const char *param_name)
{
    if (!filename || !param_name)
    {
        return -1;
    }
    return 0;
}
