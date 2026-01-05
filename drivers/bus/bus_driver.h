#ifndef BUS_DRIVER_H
#define BUS_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t bus_id;
    uint32_t flags;
    const char *name;
} bus_type_t;

typedef struct
{
    uint32_t id;
    uint32_t bus_id;
    uint32_t vendor_id;
    uint32_t device_id;
} bus_device_t;

int bus_init(void);
int bus_register_type(bus_type_t *bus);
int bus_add_device(bus_device_t *dev);
int bus_remove_device(uint32_t id);
int bus_scan(uint32_t bus_id);

#endif
