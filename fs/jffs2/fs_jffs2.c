#include "fs_jffs2.h"

int fs_jffs2_init(void)
{
    return 0;
}

int fs_jffs2_mount(fs_jffs2_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_jffs2_scan_flash(fs_jffs2_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_jffs2_read_inode(fs_jffs2_device_t *dev, fs_jffs2_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_jffs2_write_inode(fs_jffs2_device_t *dev, fs_jffs2_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_jffs2_garbage_collect(fs_jffs2_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
