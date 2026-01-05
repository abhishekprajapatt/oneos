#include "fs_btrfs.h"

int fs_btrfs_init(void)
{
    return 0;
}

int fs_btrfs_mount(fs_btrfs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_btrfs_unmount(fs_btrfs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_btrfs_read_inode(fs_btrfs_device_t *dev, fs_btrfs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_btrfs_write_inode(fs_btrfs_device_t *dev, fs_btrfs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_btrfs_read_block(fs_btrfs_device_t *dev, uint64_t block, uint8_t *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
