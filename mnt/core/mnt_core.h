#ifndef MNT_CORE_H
#define MNT_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t mount_id;
    const char *source;
    const char *target;
    const char *fstype;
    uint32_t flags;
} mnt_core_mount_t;

typedef struct
{
    uint32_t total_mounts;
    uint32_t active_mounts;
    uint64_t total_size;
    uint64_t used_space;
} mnt_core_state_t;

int mnt_core_init(void);
int mnt_core_mount(mnt_core_mount_t *mount);
int mnt_core_unmount(uint32_t mount_id);
int mnt_core_get_mount_info(uint32_t mount_id, mnt_core_mount_t *mount);
int mnt_core_get_state(mnt_core_state_t *state);

#endif
