#ifndef MISC_DRIVER_H
#define MISC_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *name;
    uint32_t flags;
} misc_device_t;

int misc_init(void);
int misc_register(misc_device_t *dev);
int misc_unregister(uint32_t id);
int misc_open(misc_device_t *dev);
int misc_close(misc_device_t *dev);

#endif
