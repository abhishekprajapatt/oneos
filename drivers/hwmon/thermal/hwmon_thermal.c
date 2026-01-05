#include "hwmon_thermal.h"

int hwmon_thermal_init(void)
{
    return 0;
}

int hwmon_thermal_probe(hwmon_thermal_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int hwmon_thermal_read_temp(hwmon_thermal_device_t *dev, hwmon_thermal_reading_t *reading)
{
    if (!dev || !reading)
    {
        return -1;
    }
    return 0;
}

int hwmon_thermal_set_critical_temp(hwmon_thermal_device_t *dev, int32_t temp)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int hwmon_thermal_enable_interrupt(hwmon_thermal_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int hwmon_thermal_get_status(hwmon_thermal_device_t *dev, uint32_t *status)
{
    if (!dev || !status)
    {
        return -1;
    }
    return 0;
}
