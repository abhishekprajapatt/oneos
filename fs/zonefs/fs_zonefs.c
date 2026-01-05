#include "fs_zonefs.h"

int fs_zonefs_init(void)
{
    return 0;
}

int fs_zonefs_mount(fs_zonefs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_zonefs_unmount(fs_zonefs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_zonefs_get_zone_info(fs_zonefs_device_t *dev, uint32_t zone_num, fs_zonefs_zone_t *zone)
{
    if (!dev || !zone)
    {
        return -1;
    }
    return 0;
}

int fs_zonefs_reset_zone(fs_zonefs_device_t *dev, uint32_t zone_num)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_zonefs_write_zone(fs_zonefs_device_t *dev, uint32_t zone_num, uint8_t *data, uint32_t size)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}
