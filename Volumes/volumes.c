#include "volumes.h"

int volumes_init(void)
{
    return 0;
}

int volumes_list(volume_info_t **volumes, uint32_t *count)
{
    if (!volumes || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int volumes_get_info(const char *volume_name, volume_info_t *info)
{
    if (!volume_name || !info)
    {
        return -1;
    }
    return 0;
}

int volumes_format(const char *volume_name)
{
    if (!volume_name)
    {
        return -1;
    }
    return 0;
}

int volumes_mount(const char *volume_name, const char *mountpoint)
{
    if (!volume_name || !mountpoint)
    {
        return -1;
    }
    return 0;
}
