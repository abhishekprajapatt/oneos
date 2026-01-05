#include "serial_8250_driver.h"

int serial_8250_init(void)
{
    return 0;
}

int serial_8250_probe(void)
{
    return 0;
}

int serial_8250_open(serial_8250_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_8250_close(serial_8250_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_8250_write(serial_8250_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data || len == 0)
    {
        return -1;
    }
    return 0;
}

int serial_8250_read(serial_8250_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data || len == 0)
    {
        return -1;
    }
    return 0;
}

int serial_8250_set_baud(serial_8250_device_t *dev, uint32_t baud)
{
    if (!dev)
    {
        return -1;
    }
    dev->baud_rate = baud;
    return 0;
}
