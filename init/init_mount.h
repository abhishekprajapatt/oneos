#ifndef INIT_MOUNT_H
#define INIT_MOUNT_H

#include <stdint.h>

typedef struct
{
    uint32_t mount_id;
    const char *device;
    const char *mount_point;
    const char *filesystem_type;
    uint32_t flags;
} init_mount_entry_t;

typedef struct
{
    uint32_t mount_count;
    uint32_t mounted_count;
    uint32_t failed_count;
} init_mount_state_t;

int init_mount_load_fstab(const char *fstab_file);
int init_mount_add_entry(init_mount_entry_t *entry);
int init_mount_remove_entry(uint32_t mount_id);
int init_mount_mount_filesystem(uint32_t mount_id);
int init_mount_unmount_filesystem(uint32_t mount_id);
int init_mount_get_status(uint32_t mount_id);

#endif
