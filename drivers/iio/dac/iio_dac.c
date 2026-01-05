#include "iio_dac.h"

int iio_dac_init(void)
{
    return 0;
}

int iio_dac_probe(iio_dac_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_dac_enable_channel(iio_dac_device_t *dev, uint8_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_dac_disable_channel(iio_dac_device_t *dev, uint8_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_dac_write_output(iio_dac_device_t *dev, iio_dac_output_t *output)
{
    if (!dev || !output)
    {
        return -1;
    }
    return 0;
}

int iio_dac_set_update_rate(iio_dac_device_t *dev, uint32_t rate)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
