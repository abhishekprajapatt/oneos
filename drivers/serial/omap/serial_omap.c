#include "serial_omap.h"

int serial_omap_init(void)
{
    return 0;
}

int serial_omap_probe(serial_omap_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_omap_configure(serial_omap_device_t *dev, serial_omap_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int serial_omap_enable(serial_omap_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_omap_disable(serial_omap_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int serial_omap_transmit(serial_omap_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}
