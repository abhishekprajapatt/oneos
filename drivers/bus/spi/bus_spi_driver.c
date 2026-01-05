#include "bus_spi_driver.h"

int bus_spi_init(void)
{
    return 0;
}

int bus_spi_register_master(uint32_t bus_id, uint32_t num_devices)
{
    return 0;
}

int bus_spi_register_device(bus_spi_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int bus_spi_transfer(bus_spi_device_t *dev, bus_spi_transfer_t *xfer)
{
    if (!dev || !xfer)
    {
        return -1;
    }
    return 0;
}

int bus_spi_write(bus_spi_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data || len == 0)
    {
        return -1;
    }
    return 0;
}

int bus_spi_read(bus_spi_device_t *dev, uint8_t *data, uint32_t len)
{
    if (!dev || !data || len == 0)
    {
        return -1;
    }
    return 0;
}
