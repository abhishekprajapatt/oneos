#include "sbin_umount.h"

int sbin_umount_init(void)
{
    return 0;
}

int sbin_umount_unmount(sbin_umount_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_umount_is_mounted(const char *target)
{
    if (!target)
    {
        return -1;
    }
    return 0;
}

int sbin_umount_get_status(uint32_t umount_id, sbin_umount_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_umount_get_state(sbin_umount_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
