#ifndef TTY_DRIVER_H
#define TTY_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t flags;
    uint32_t buffer_size;
} tty_device_t;

typedef struct
{
    uint8_t *buffer;
    uint32_t length;
} tty_buffer_t;

int tty_init(void);
int tty_register_device(tty_device_t *dev);
int tty_write(tty_device_t *dev, tty_buffer_t *buf);
int tty_read(tty_device_t *dev, tty_buffer_t *buf);
int tty_ioctl(tty_device_t *dev, uint32_t cmd, void *arg);

#endif
