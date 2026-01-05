#ifndef SERIAL_OMAP_H
#define SERIAL_OMAP_H

#include <stdint.h>

typedef struct
{
    uint32_t port;
    uint32_t base_address;
    uint32_t irq;
    uint32_t flags;
    uint32_t clk_rate;
} serial_omap_device_t;

typedef struct
{
    uint32_t baud;
    uint8_t bits;
    uint8_t parity;
    uint8_t stopbits;
    uint32_t flow_control;
} serial_omap_config_t;

int serial_omap_init(void);
int serial_omap_probe(serial_omap_device_t *dev);
int serial_omap_configure(serial_omap_device_t *dev, serial_omap_config_t *cfg);
int serial_omap_enable(serial_omap_device_t *dev);
int serial_omap_disable(serial_omap_device_t *dev);
int serial_omap_transmit(serial_omap_device_t *dev, uint8_t *data, uint32_t len);

#endif
