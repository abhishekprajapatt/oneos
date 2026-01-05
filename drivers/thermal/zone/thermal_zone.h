#ifndef THERMAL_ZONE_H
#define THERMAL_ZONE_H

#include <stdint.h>

typedef struct
{
    uint32_t zone_id;
    int32_t temp_threshold;
    int32_t trip_point;
    uint8_t sensor_count;
} thermal_zone_device_t;

typedef struct
{
    int32_t current_temp;
    uint32_t timestamp;
} thermal_zone_reading_t;

int thermal_zone_init(void);
int thermal_zone_probe(thermal_zone_device_t *dev);
int thermal_zone_read_temp(thermal_zone_device_t *dev, thermal_zone_reading_t *reading);
int thermal_zone_bind_sensor(thermal_zone_device_t *dev, uint32_t sensor_id);
int thermal_zone_unbind_sensor(thermal_zone_device_t *dev, uint32_t sensor_id);
int thermal_zone_set_trip_point(thermal_zone_device_t *dev, int32_t temp);

#endif
