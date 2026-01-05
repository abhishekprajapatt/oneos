#include "serial_8250.h"

int serial_8250_init(void)
{
    return 0;
}

int serial_8250_probe(serial_8250_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_8250_configure(serial_8250_device_t *dev, serial_8250_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int serial_8250_transmit(serial_8250_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}

int serial_8250_receive(serial_8250_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}

int serial_8250_set_modem_control(serial_8250_device_t *dev, uint8_t mcr)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
