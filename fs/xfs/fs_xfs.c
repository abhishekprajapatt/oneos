#include "fs_xfs.h"

int fs_xfs_init(void)
{
    return 0;
}

int fs_xfs_mount(fs_xfs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_xfs_unmount(fs_xfs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_xfs_read_inode(fs_xfs_device_t *dev, fs_xfs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_xfs_write_inode(fs_xfs_device_t *dev, fs_xfs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_xfs_allocate_extent(fs_xfs_device_t *dev, uint64_t *extent)
{
    if (!dev || !extent)
    {
        return -1;
    }
    return 0;
}
