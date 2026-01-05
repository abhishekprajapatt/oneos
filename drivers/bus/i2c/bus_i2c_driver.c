#include "bus_i2c_driver.h"

int bus_i2c_init(void)
{
    return 0;
}

int bus_i2c_add_adapter(uint32_t adapter_id)
{
    return 0;
}

int bus_i2c_detect_devices(uint32_t adapter_id)
{
    return 0;
}

int bus_i2c_transfer(uint32_t adapter_id, bus_i2c_message_t *msg, uint32_t count)
{
    if (!msg)
    {
        return -1;
    }
    return 0;
}

int bus_i2c_write(uint32_t adapter_id, uint8_t addr, uint8_t *data, uint32_t len)
{
    if (!data || len == 0)
    {
        return -1;
    }
    return 0;
}

int bus_i2c_read(uint32_t adapter_id, uint8_t addr, uint8_t *data, uint32_t len)
{
    if (!data || len == 0)
    {
        return -1;
    }
    return 0;
}
