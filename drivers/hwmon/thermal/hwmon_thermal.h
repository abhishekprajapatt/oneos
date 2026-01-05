#ifndef HWMON_THERMAL_H
#define HWMON_THERMAL_H

#include <stdint.h>

typedef struct
{
    uint32_t sensor_id;
    uint32_t base_address;
    int32_t critical_temp;
    int32_t shutdown_temp;
} hwmon_thermal_device_t;

typedef struct
{
    int32_t current_temp;
    int32_t max_temp;
    int32_t min_temp;
    uint32_t timestamp;
} hwmon_thermal_reading_t;

int hwmon_thermal_init(void);
int hwmon_thermal_probe(hwmon_thermal_device_t *dev);
int hwmon_thermal_read_temp(hwmon_thermal_device_t *dev, hwmon_thermal_reading_t *reading);
int hwmon_thermal_set_critical_temp(hwmon_thermal_device_t *dev, int32_t temp);
int hwmon_thermal_enable_interrupt(hwmon_thermal_device_t *dev);
int hwmon_thermal_get_status(hwmon_thermal_device_t *dev, uint32_t *status);

#endif
