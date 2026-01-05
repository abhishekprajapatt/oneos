#ifndef SENSORS_ACCEL_H
#define SENSORS_ACCEL_H

#include <stdint.h>

typedef struct
{
    uint32_t sensor_id;
    uint32_t base_address;
    uint8_t axis_count;
    uint32_t full_scale;
} sensors_accel_device_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
    uint64_t timestamp;
} sensors_accel_data_t;

int sensors_accel_init(void);
int sensors_accel_probe(sensors_accel_device_t *dev);
int sensors_accel_read_data(sensors_accel_device_t *dev, sensors_accel_data_t *data);
int sensors_accel_set_full_scale(sensors_accel_device_t *dev, uint32_t full_scale);
int sensors_accel_calibrate(sensors_accel_device_t *dev);
int sensors_accel_enable_tap_detection(sensors_accel_device_t *dev);

#endif
