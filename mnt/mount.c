#include "mount.h"

int mount_init(void)
{
    return 0;
}

int mount_device(const char *device, const char *mountpoint, const char *fstype)
{
    if (!device || !mountpoint || !fstype)
    {
        return -1;
    }
    return 0;
}

int mount_unmount(const char *mountpoint)
{
    if (!mountpoint)
    {
        return -1;
    }
    return 0;
}

int mount_list(char **mounts, uint32_t max_mounts)
{
    if (!mounts || max_mounts == 0)
    {
        return -1;
    }
    return 0;
}

int mount_get_info(const char *mountpoint, void **info)
{
    if (!mountpoint || !info)
    {
        return -1;
    }
    return 0;
}
