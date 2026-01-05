#ifndef THERMAL_DRIVER_H
#define THERMAL_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t zone_id;
    int32_t temperature;
    int32_t trip_point;
    uint32_t governor;
} thermal_zone_t;

int thermal_init(void);
int thermal_register_zone(thermal_zone_t *zone);
int thermal_get_temperature(uint32_t zone_id, int32_t *temp);
int thermal_set_trip_point(uint32_t zone_id, int32_t trip);
int thermal_cool(uint32_t zone_id, uint32_t level);

#endif
