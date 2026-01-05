#include "iio_adc.h"

int iio_adc_init(void)
{
    return 0;
}

int iio_adc_probe(iio_adc_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_adc_enable_channel(iio_adc_device_t *dev, uint8_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_adc_disable_channel(iio_adc_device_t *dev, uint8_t channel)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_adc_read_sample(iio_adc_device_t *dev, iio_adc_sample_t *sample)
{
    if (!dev || !sample)
    {
        return -1;
    }
    return 0;
}

int iio_adc_set_sample_rate(iio_adc_device_t *dev, uint32_t rate)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
