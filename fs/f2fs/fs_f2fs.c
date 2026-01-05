#include "fs_f2fs.h"

int fs_f2fs_init(void)
{
    return 0;
}

int fs_f2fs_mount(fs_f2fs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_f2fs_unmount(fs_f2fs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_f2fs_read_inode(fs_f2fs_device_t *dev, fs_f2fs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_f2fs_write_inode(fs_f2fs_device_t *dev, fs_f2fs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_f2fs_allocate_segment(fs_f2fs_device_t *dev, uint32_t *segment)
{
    if (!dev || !segment)
    {
        return -1;
    }
    return 0;
}
