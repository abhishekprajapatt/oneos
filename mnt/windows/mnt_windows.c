#include "mnt_windows.h"

int mnt_windows_init(void)
{
    return 0;
}

int mnt_windows_mount_drive(mnt_windows_mount_t *mount)
{
    if (!mount)
    {
        return -1;
    }
    return 0;
}

int mnt_windows_unmount_drive(uint32_t win_mount_id)
{
    return 0;
}

int mnt_windows_map_network_share(const char *path, const char *drive_letter)
{
    if (!path || !drive_letter)
    {
        return -1;
    }
    return 0;
}

int mnt_windows_unmap_network_share(const char *drive_letter)
{
    if (!drive_letter)
    {
        return -1;
    }
    return 0;
}

int mnt_windows_get_state(mnt_windows_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
