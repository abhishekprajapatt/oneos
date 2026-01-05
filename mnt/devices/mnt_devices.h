#ifndef MNT_DEVICES_H
#define MNT_DEVICES_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_path;
    const char *device_name;
    uint64_t capacity;
    uint32_t mount_status;
} mnt_devices_entry_t;

typedef struct
{
    uint32_t device_count;
    uint32_t mounted_devices;
    uint32_t unmounted_devices;
    uint64_t total_capacity;
} mnt_devices_state_t;

int mnt_devices_init(void);
int mnt_devices_register(mnt_devices_entry_t *device);
int mnt_devices_unregister(uint32_t device_id);
int mnt_devices_get_info(uint32_t device_id, mnt_devices_entry_t *device);
int mnt_devices_scan(void);
int mnt_devices_get_state(mnt_devices_state_t *state);

#endif
