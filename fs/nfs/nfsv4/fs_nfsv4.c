#include "fs_nfsv4.h"

int fs_nfsv4_init(void)
{
    return 0;
}

int fs_nfsv4_connect(fs_nfsv4_device_t *dev, const char *clientid)
{
    if (!dev || !clientid)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv4_disconnect(fs_nfsv4_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv4_lookup(fs_nfsv4_device_t *dev, const char *path)
{
    if (!dev || !path)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv4_read_file(fs_nfsv4_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}

int fs_nfsv4_write_file(fs_nfsv4_device_t *dev, uint64_t inode, uint64_t offset, uint8_t *buffer, uint32_t size)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
