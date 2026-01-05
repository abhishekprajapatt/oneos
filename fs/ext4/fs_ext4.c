#include "fs_ext4.h"

int fs_ext4_init(void)
{
    return 0;
}

int fs_ext4_mount(fs_ext4_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_ext4_unmount(fs_ext4_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_ext4_read_inode(fs_ext4_device_t *dev, fs_ext4_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_ext4_write_inode(fs_ext4_device_t *dev, fs_ext4_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_ext4_find_extent(fs_ext4_device_t *dev, uint32_t inode, uint64_t block)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
