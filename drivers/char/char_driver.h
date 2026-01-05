#ifndef CHAR_DRIVER_H
#define CHAR_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t flags;
    uint32_t buffer_size;
} char_device_t;

typedef struct
{
    uint8_t *buffer;
    uint32_t offset;
    uint32_t size;
} char_request_t;

int char_init(void);
int char_open(char_device_t *dev);
int char_close(char_device_t *dev);
int char_read(char_device_t *dev, char_request_t *req);
int char_write(char_device_t *dev, char_request_t *req);
int char_ioctl(char_device_t *dev, uint32_t cmd, void *arg);

#endif
