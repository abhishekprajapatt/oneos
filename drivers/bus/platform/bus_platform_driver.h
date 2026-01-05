#ifndef BUS_PLATFORM_DRIVER_H
#define BUS_PLATFORM_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *name;
    uint32_t flags;
    uint32_t *resource;
    uint32_t num_resources;
} bus_platform_device_t;

typedef struct
{
    const char *name;
    int (*probe)(bus_platform_device_t *dev);
    int (*remove)(bus_platform_device_t *dev);
} bus_platform_driver_t;

int bus_platform_init(void);
int bus_platform_device_register(bus_platform_device_t *dev);
int bus_platform_device_unregister(uint32_t id);
int bus_platform_driver_register(bus_platform_driver_t *driver);
int bus_platform_driver_unregister(const char *name);
int bus_platform_add_resources(bus_platform_device_t *dev, uint32_t *res, uint32_t count);

#endif
