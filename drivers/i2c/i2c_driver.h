#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t bus_id;
    uint32_t speed;
} i2c_adapter_t;

typedef struct
{
    uint8_t addr;
    uint8_t *buffer;
    uint32_t length;
    uint32_t flags;
} i2c_msg_t;

int i2c_init(void);
int i2c_add_adapter(i2c_adapter_t *adapter);
int i2c_transfer(i2c_adapter_t *adapter, i2c_msg_t *msg, uint32_t count);
int i2c_read(i2c_adapter_t *adapter, uint8_t addr, uint8_t *data, uint32_t len);
int i2c_write(i2c_adapter_t *adapter, uint8_t addr, uint8_t *data, uint32_t len);

#endif
