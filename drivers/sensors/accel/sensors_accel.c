#include "sensors_accel.h"

int sensors_accel_init(void)
{
    return 0;
}

int sensors_accel_probe(sensors_accel_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sensors_accel_read_data(sensors_accel_device_t *dev, sensors_accel_data_t *data)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}

int sensors_accel_set_full_scale(sensors_accel_device_t *dev, uint32_t full_scale)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sensors_accel_calibrate(sensors_accel_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sensors_accel_enable_tap_detection(sensors_accel_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
