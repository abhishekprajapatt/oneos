#include "iio_driver.h"

int iio_init(void)
{
    return 0;
}

int iio_register_device(iio_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_read_channel(iio_device_t *dev, uint32_t channel, int32_t *value)
{
    if (!dev || !value)
    {
        return -1;
    }
    return 0;
}

int iio_enable_channel(iio_device_t *dev, uint32_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_disable_channel(iio_device_t *dev, uint32_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
