#ifndef PLATFORM_DRIVER_H
#define PLATFORM_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    const char *name;
    uint32_t flags;
} platform_device_t;

int platform_init(void);
int platform_device_register(platform_device_t *dev);
int platform_device_unregister(uint32_t id);
int platform_driver_register(const char *name);
int platform_driver_unregister(const char *name);

#endif
