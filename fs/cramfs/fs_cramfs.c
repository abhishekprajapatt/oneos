#include "fs_cramfs.h"

int fs_cramfs_init(void)
{
    return 0;
}

int fs_cramfs_mount(fs_cramfs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_cramfs_read_inode(fs_cramfs_device_t *dev, uint32_t inode_num, fs_cramfs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_cramfs_read_block(fs_cramfs_device_t *dev, uint32_t block, uint8_t *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_cramfs_get_file_size(fs_cramfs_device_t *dev, uint32_t inode, uint32_t *size)
{
    if (!dev || !size)
    {
        return -1;
    }
    return 0;
}

int fs_cramfs_read_file(fs_cramfs_device_t *dev, uint32_t inode, uint8_t *buffer, uint32_t size)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
