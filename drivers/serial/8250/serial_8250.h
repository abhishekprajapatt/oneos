#ifndef SERIAL_8250_H
#define SERIAL_8250_H

#include <stdint.h>

typedef struct
{
    uint32_t port;
    uint32_t base_address;
    uint32_t irq;
    uint32_t baudrate;
    uint8_t lcr;
    uint8_t fcr;
} serial_8250_device_t;

typedef struct
{
    uint32_t baud;
    uint8_t bits;
    uint8_t parity;
    uint8_t stopbits;
} serial_8250_config_t;

int serial_8250_init(void);
int serial_8250_probe(serial_8250_device_t *dev);
int serial_8250_configure(serial_8250_device_t *dev, serial_8250_config_t *cfg);
int serial_8250_transmit(serial_8250_device_t *dev, uint8_t *data, uint32_t len);
int serial_8250_receive(serial_8250_device_t *dev, uint8_t *data, uint32_t len);
int serial_8250_set_modem_control(serial_8250_device_t *dev, uint8_t mcr);

#endif
