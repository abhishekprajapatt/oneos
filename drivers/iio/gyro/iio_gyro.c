#include "iio_gyro.h"

int iio_gyro_init(void)
{
    return 0;
}

int iio_gyro_probe(iio_gyro_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_gyro_read_data(iio_gyro_device_t *dev, iio_gyro_data_t *data)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}

int iio_gyro_set_full_scale(iio_gyro_device_t *dev, uint32_t full_scale)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_gyro_calibrate(iio_gyro_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int iio_gyro_enable_interrupt(iio_gyro_device_t *dev, uint8_t irq_type)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
