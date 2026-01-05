#include "mnt_core.h"

int mnt_core_init(void)
{
    return 0;
}

int mnt_core_mount(mnt_core_mount_t *mount)
{
    if (!mount)
    {
        return -1;
    }
    return 0;
}

int mnt_core_unmount(uint32_t mount_id)
{
    return 0;
}

int mnt_core_get_mount_info(uint32_t mount_id, mnt_core_mount_t *mount)
{
    if (!mount)
    {
        return -1;
    }
    return 0;
}

int mnt_core_get_state(mnt_core_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
