#include "etc_oneos.h"

int etc_oneos_init(void)
{
    return 0;
}

int etc_oneos_load_config(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}

int etc_oneos_save_config(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}

int etc_oneos_get_config(const char *key, char *value, uint32_t size)
{
    if (!key || !value)
    {
        return -1;
    }
    return 0;
}

int etc_oneos_set_config(const char *key, const char *value)
{
    if (!key || !value)
    {
        return -1;
    }
    return 0;
}

int etc_oneos_reload(void)
{
    return 0;
}
