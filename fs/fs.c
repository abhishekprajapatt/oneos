#include "fs.h"

int fs_init(void)
{
    return 0;
}

int fs_mount(const char *device, const char *mountpoint)
{
    if (!device || !mountpoint)
    {
        return -1;
    }
    return 0;
}

int fs_umount(const char *mountpoint)
{
    if (!mountpoint)
    {
        return -1;
    }
    return 0;
}

int fs_stat(const char *path, fs_stat_t *stat)
{
    if (!path || !stat)
    {
        return -1;
    }
    stat->block_size = 4096;
    stat->total_blocks = 1000000;
    stat->free_blocks = 500000;
    return 0;
}

int fs_mkdir(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int fs_rmdir(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int fs_open(const char *path, int flags, void **fd)
{
    if (!path || !fd)
    {
        return -1;
    }
    return 0;
}

int fs_close(void *fd)
{
    if (!fd)
    {
        return -1;
    }
    return 0;
}
