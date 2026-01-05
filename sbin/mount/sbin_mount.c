#include "sbin_mount.h"

int sbin_mount_init(void)
{
    return 0;
}

int sbin_mount_mount(sbin_mount_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_mount_unmount(const char *target)
{
    if (!target)
    {
        return -1;
    }
    return 0;
}

int sbin_mount_get_mounted(sbin_mount_entry_t *mounts, uint32_t *count)
{
    if (!mounts || !count)
    {
        return -1;
    }
    return 0;
}

int sbin_mount_get_state(sbin_mount_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
