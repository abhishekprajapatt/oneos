#include "fs_nfsv3.h"

int fs_nfsv3_init(void)
{
    return 0;
}

int fs_nfsv3_connect(fs_nfsv3_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv3_disconnect(fs_nfsv3_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv3_lookup(fs_nfsv3_device_t *dev, const char *path)
{
    if (!dev || !path)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv3_read_file(fs_nfsv3_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv3_write_file(fs_nfsv3_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
