#ifndef SBIN_MOUNT_H
#define SBIN_MOUNT_H

#include <stdint.h>

typedef struct
{
    uint32_t mount_id;
    const char *source;
    const char *target;
    const char *fstype;
    uint32_t mount_status;
} sbin_mount_entry_t;

typedef struct
{
    uint32_t total_mounts;
    uint32_t active_mounts;
    uint32_t failed_mounts;
} sbin_mount_state_t;

int sbin_mount_init(void);
int sbin_mount_mount(sbin_mount_entry_t *entry);
int sbin_mount_unmount(const char *target);
int sbin_mount_get_mounted(sbin_mount_entry_t *mounts, uint32_t *count);
int sbin_mount_get_state(sbin_mount_state_t *state);

#endif
