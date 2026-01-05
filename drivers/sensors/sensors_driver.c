#include "sensors_driver.h"

int sensors_init(void)
{
    return 0;
}

int sensors_register(sensor_t *sensor)
{
    if (!sensor)
    {
        return -1;
    }
    return 0;
}

int sensors_read(uint32_t id, int32_t *value)
{
    if (!value)
    {
        return -1;
    }
    *value = 0;
    return 0;
}

int sensors_set_threshold(uint32_t id, int32_t min, int32_t max)
{
    return 0;
}
