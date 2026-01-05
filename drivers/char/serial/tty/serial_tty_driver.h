#ifndef SERIAL_TTY_DRIVER_H
#define SERIAL_TTY_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t flags;
    uint32_t buffer_size;
} serial_tty_device_t;

typedef struct
{
    uint32_t line;
    uint32_t type;
    uint32_t flags;
    const char *name;
} serial_tty_info_t;

int serial_tty_init(void);
int serial_tty_register_driver(serial_tty_info_t *info);
int serial_tty_unregister_driver(uint32_t line);
int serial_tty_open(serial_tty_device_t *dev);
int serial_tty_close(serial_tty_device_t *dev);
int serial_tty_write(serial_tty_device_t *dev, uint8_t *data, uint32_t len);
int serial_tty_read(serial_tty_device_t *dev, uint8_t *data, uint32_t len);

#endif
