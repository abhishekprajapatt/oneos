#ifndef SERIAL_DRIVER_H
#define SERIAL_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
} serial_config_t;

typedef struct
{
    uint32_t device_id;
    serial_config_t config;
} serial_port_t;

int serial_init(void);
int serial_open(serial_port_t *port);
int serial_close(serial_port_t *port);
int serial_configure(serial_port_t *port, serial_config_t *cfg);
int serial_write(serial_port_t *port, uint8_t *data, uint32_t len);
int serial_read(serial_port_t *port, uint8_t *data, uint32_t len);

#endif
