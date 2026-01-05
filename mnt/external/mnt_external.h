#ifndef MNT_EXTERNAL_H
#define MNT_EXTERNAL_H

#include <stdint.h>

typedef struct
{
    uint32_t external_id;
    const char *mount_path;
    const char *device_type;
    uint32_t connection_type;
    uint32_t status;
} mnt_external_device_t;

typedef struct
{
    uint32_t connected_devices;
    uint32_t mounted_external;
    uint32_t pending_mount;
    uint64_t total_external_capacity;
} mnt_external_state_t;

int mnt_external_init(void);
int mnt_external_connect(mnt_external_device_t *device);
int mnt_external_disconnect(uint32_t external_id);
int mnt_external_mount(uint32_t external_id);
int mnt_external_unmount(uint32_t external_id);
int mnt_external_get_state(mnt_external_state_t *state);

#endif
