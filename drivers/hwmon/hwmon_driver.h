#ifndef HWMON_DRIVER_H
#define HWMON_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    int32_t temperature;
    uint32_t voltage;
    uint32_t current;
} hwmon_sensor_t;

int hwmon_init(void);
int hwmon_register_sensor(hwmon_sensor_t *sensor);
int hwmon_read_temperature(uint32_t id, int32_t *temp);
int hwmon_read_voltage(uint32_t id, uint32_t *voltage);
int hwmon_read_current(uint32_t id, uint32_t *current);

#endif
