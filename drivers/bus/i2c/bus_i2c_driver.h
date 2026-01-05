#ifndef BUS_I2C_DRIVER_H
#define BUS_I2C_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t adapter_id;
    uint8_t addr;
    uint32_t flags;
} bus_i2c_device_t;

typedef struct
{
    uint8_t addr;
    uint8_t *buffer;
    uint32_t length;
    uint32_t flags;
} bus_i2c_message_t;

int bus_i2c_init(void);
int bus_i2c_add_adapter(uint32_t adapter_id);
int bus_i2c_detect_devices(uint32_t adapter_id);
int bus_i2c_transfer(uint32_t adapter_id, bus_i2c_message_t *msg, uint32_t count);
int bus_i2c_write(uint32_t adapter_id, uint8_t addr, uint8_t *data, uint32_t len);
int bus_i2c_read(uint32_t adapter_id, uint8_t addr, uint8_t *data, uint32_t len);

#endif
