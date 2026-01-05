#include "kernel_fs.h"

int kernel_fs_init(void)
{
    return 0;
}

int kernel_fs_register(kernel_fs_info_t *fs_info)
{
    if (!fs_info)
    {
        return -1;
    }
    return 0;
}

int kernel_fs_unregister(uint32_t fs_id)
{
    return 0;
}

int kernel_fs_mount(uint32_t fs_id, const char *mount_point)
{
    if (!mount_point)
    {
        return -1;
    }
    return 0;
}

int kernel_fs_unmount(uint32_t fs_id)
{
    return 0;
}

int kernel_fs_get_info(uint32_t fs_id, kernel_fs_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}
