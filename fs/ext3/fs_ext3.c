#include "fs_ext3.h"

int fs_ext3_init(void)
{
    return 0;
}

int fs_ext3_mount(fs_ext3_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_ext3_unmount(fs_ext3_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_ext3_read_inode(fs_ext3_device_t *dev, fs_ext3_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_ext3_write_inode(fs_ext3_device_t *dev, fs_ext3_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_ext3_recover_journal(fs_ext3_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
