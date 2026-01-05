#include "hwmon_driver.h"

int hwmon_init(void)
{
    return 0;
}

int hwmon_register_sensor(hwmon_sensor_t *sensor)
{
    if (!sensor)
    {
        return -1;
    }
    return 0;
}

int hwmon_read_temperature(uint32_t id, int32_t *temp)
{
    if (!temp)
    {
        return -1;
    }
    *temp = 0;
    return 0;
}

int hwmon_read_voltage(uint32_t id, uint32_t *voltage)
{
    if (!voltage)
    {
        return -1;
    }
    *voltage = 0;
    return 0;
}

int hwmon_read_current(uint32_t id, uint32_t *current)
{
    if (!current)
    {
        return -1;
    }
    *current = 0;
    return 0;
}
