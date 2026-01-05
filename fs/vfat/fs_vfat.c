#include "fs_vfat.h"

int fs_vfat_init(void)
{
    return 0;
}

int fs_vfat_mount(fs_vfat_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_vfat_read_fat(fs_vfat_device_t *dev, uint32_t fat_entry)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_vfat_read_cluster(fs_vfat_device_t *dev, uint32_t cluster, uint8_t *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_vfat_read_inode(fs_vfat_device_t *dev, fs_vfat_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_vfat_read_file(fs_vfat_device_t *dev, fs_vfat_inode_t *inode, uint8_t *buffer, uint32_t size)
{
    if (!dev || !inode || !buffer)
    {
        return -1;
    }
    return 0;
}
