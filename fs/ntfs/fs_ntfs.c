#include "fs_ntfs.h"

int fs_ntfs_init(void)
{
    return 0;
}

int fs_ntfs_mount(fs_ntfs_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_ntfs_read_mft_record(fs_ntfs_device_t *dev, uint32_t record_num, uint8_t *buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_ntfs_read_inode(fs_ntfs_device_t *dev, fs_ntfs_inode_t *inode)
{
    if (!dev || !inode)
    {
        return -1;
    }
    return 0;
}

int fs_ntfs_read_file(fs_ntfs_device_t *dev, uint64_t mft_ref, uint8_t *buffer, uint32_t size)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_ntfs_list_directory(fs_ntfs_device_t *dev, uint64_t mft_ref)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
