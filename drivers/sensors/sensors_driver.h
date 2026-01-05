#ifndef SENSORS_DRIVER_H
#define SENSORS_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    int32_t min_value;
    int32_t max_value;
    int32_t current_value;
} sensor_t;

int sensors_init(void);
int sensors_register(sensor_t *sensor);
int sensors_read(uint32_t id, int32_t *value);
int sensors_set_threshold(uint32_t id, int32_t min, int32_t max);

#endif
