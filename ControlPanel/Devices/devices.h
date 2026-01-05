#ifndef DEVICES_H
#define DEVICES_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    const char *device_type;
    uint32_t is_connected;
    uint32_t is_enabled;
} device_info_t;

typedef struct
{
    uint32_t total_devices;
    uint32_t connected_devices;
    uint32_t enabled_devices;
    uint32_t disabled_devices;
} devices_stats_t;

int devices_init(void);
int devices_list(device_info_t *devices, uint32_t *count);
int devices_get_info(uint32_t device_id, device_info_t *info);
int devices_enable(uint32_t device_id);
int devices_disable(uint32_t device_id);
int devices_refresh_list(void);
int devices_get_properties(uint32_t device_id, char *properties, uint32_t max_size);
int devices_get_stats(devices_stats_t *stats);

#endif
