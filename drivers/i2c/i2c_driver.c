#include "i2c_driver.h"

int i2c_init(void)
{
    return 0;
}

int i2c_add_adapter(i2c_adapter_t *adapter)
{
    if (!adapter)
    {
        return -1;
    }
    return 0;
}

int i2c_transfer(i2c_adapter_t *adapter, i2c_msg_t *msg, uint32_t count)
{
    if (!adapter || !msg)
    {
        return -1;
    }
    return 0;
}

int i2c_read(i2c_adapter_t *adapter, uint8_t addr, uint8_t *data, uint32_t len)
{
    if (!adapter || !data)
    {
        return -1;
    }
    return 0;
}

int i2c_write(i2c_adapter_t *adapter, uint8_t addr, uint8_t *data, uint32_t len)
{
    if (!adapter || !data)
    {
        return -1;
    }
    return 0;
}
