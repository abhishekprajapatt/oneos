#include "mount_utilities.h"

int mount_utilities_init(void)
{
    return 0;
}

int mount_utilities_mount_filesystem(const char *device, const char *mount_point)
{
    if (!device || !mount_point)
    {
        return -1;
    }
    return 0;
}

int mount_utilities_unmount_filesystem(const char *mount_point)
{
    if (!mount_point)
    {
        return -1;
    }
    return 0;
}

int mount_utilities_list_mounts(void)
{
    return 0;
}

int mount_utilities_get_mount_info(const char *mount_point)
{
    if (!mount_point)
    {
        return -1;
    }
    return 0;
}

int mount_utilities_check_filesystem(const char *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}
