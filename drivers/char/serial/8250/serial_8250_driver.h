#ifndef SERIAL_8250_DRIVER_H
#define SERIAL_8250_DRIVER_H

#include <stdint.h>

#define UART_FIFO_SIZE 16

typedef struct
{
    uint32_t port;
    uint32_t irq;
    uint32_t baud_rate;
    uint8_t data_bits;
    uint8_t stop_bits;
    uint8_t parity;
} serial_8250_device_t;

typedef struct
{
    uint8_t *buffer;
    uint32_t length;
    uint32_t offset;
} serial_8250_buffer_t;

int serial_8250_init(void);
int serial_8250_probe(void);
int serial_8250_open(serial_8250_device_t *dev);
int serial_8250_close(serial_8250_device_t *dev);
int serial_8250_write(serial_8250_device_t *dev, uint8_t *data, uint32_t len);
int serial_8250_read(serial_8250_device_t *dev, uint8_t *data, uint32_t len);
int serial_8250_set_baud(serial_8250_device_t *dev, uint32_t baud);

#endif
