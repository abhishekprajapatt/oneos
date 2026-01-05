#include "thermal_driver.h"

int thermal_init(void)
{
    return 0;
}

int thermal_register_zone(thermal_zone_t *zone)
{
    if (!zone)
    {
        return -1;
    }
    return 0;
}

int thermal_get_temperature(uint32_t zone_id, int32_t *temp)
{
    if (!temp)
    {
        return -1;
    }
    *temp = 0;
    return 0;
}

int thermal_set_trip_point(uint32_t zone_id, int32_t trip)
{
    return 0;
}

int thermal_cool(uint32_t zone_id, uint32_t level)
{
    return 0;
}
