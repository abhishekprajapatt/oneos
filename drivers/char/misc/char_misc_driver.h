#ifndef CHAR_MISC_DRIVER_H
#define CHAR_MISC_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *name;
    uint32_t minor;
    uint32_t flags;
} char_misc_device_t;

typedef struct
{
    uint8_t *buffer;
    uint32_t length;
    uint32_t flags;
} char_misc_request_t;

int char_misc_init(void);
int char_misc_register(char_misc_device_t *dev);
int char_misc_unregister(uint32_t device_id);
int char_misc_open(char_misc_device_t *dev);
int char_misc_close(char_misc_device_t *dev);
int char_misc_ioctl(char_misc_device_t *dev, uint32_t cmd, void *arg);

#endif
