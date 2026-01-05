#include "fs_ext2.h"

int fs_ext2_init(void)
{
    return 0;
}

int fs_ext2_mount(fs_ext2_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_ext2_unmount(fs_ext2_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_ext2_read_inode(fs_ext2_device_t *dev, fs_ext2_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_ext2_write_inode(fs_ext2_device_t *dev, fs_ext2_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_ext2_allocate_block(fs_ext2_device_t *dev, uint32_t *block)
{
    if (!dev || !block)
    {
        return -1;
    }
    return 0;
}
