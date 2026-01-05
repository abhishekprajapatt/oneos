#ifndef BASE_DRIVER_H
#define BASE_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t type;
    uint32_t status;
} base_device_t;

typedef struct
{
    uint32_t bus_id;
    uint32_t device_count;
    base_device_t *devices;
} base_bus_t;

int base_init(void);
int base_add_device(base_bus_t *bus, base_device_t *dev);
int base_remove_device(base_bus_t *bus, uint32_t id);
int base_get_device(base_bus_t *bus, uint32_t id, base_device_t *dev);

#endif
