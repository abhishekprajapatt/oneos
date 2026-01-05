#include "thermal_zone.h"

int thermal_zone_init(void)
{
    return 0;
}

int thermal_zone_probe(thermal_zone_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int thermal_zone_read_temp(thermal_zone_device_t *dev, thermal_zone_reading_t *reading)
{
    if (!dev || !reading)
    {
        return -1;
    }
    return 0;
}

int thermal_zone_bind_sensor(thermal_zone_device_t *dev, uint32_t sensor_id)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int thermal_zone_unbind_sensor(thermal_zone_device_t *dev, uint32_t sensor_id)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int thermal_zone_set_trip_point(thermal_zone_device_t *dev, int32_t temp)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
