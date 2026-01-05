#include "init_mount.h"

int init_mount_load_fstab(const char *fstab_file)
{
    if (!fstab_file)
    {
        return -1;
    }
    return 0;
}

int init_mount_add_entry(init_mount_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int init_mount_remove_entry(uint32_t mount_id)
{
    return 0;
}

int init_mount_mount_filesystem(uint32_t mount_id)
{
    return 0;
}

int init_mount_unmount_filesystem(uint32_t mount_id)
{
    return 0;
}

int init_mount_get_status(uint32_t mount_id)
{
    return 0;
}
