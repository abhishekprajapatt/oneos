#ifndef SYS_DEVICES_H
#define SYS_DEVICES_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    const char *device_path;
    uint32_t device_class;
    uint32_t status;
} sys_devices_entry_t;

typedef struct
{
    uint32_t total_devices;
    uint32_t active_devices;
    uint32_t inactive_devices;
    uint32_t error_devices;
} sys_devices_state_t;

int sys_devices_init(void);
int sys_devices_register(sys_devices_entry_t *device);
int sys_devices_unregister(uint32_t device_id);
int sys_devices_get_device(uint32_t device_id, sys_devices_entry_t *device);
int sys_devices_enumerate(sys_devices_entry_t *devices, uint32_t *count);
int sys_devices_get_state(sys_devices_state_t *state);

#endif
