#ifndef IIO_GYRO_H
#define IIO_GYRO_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint8_t axis_count;
    uint32_t full_scale;
    uint32_t sensitivity;
} iio_gyro_device_t;

typedef struct
{
    int16_t x;
    int16_t y;
    int16_t z;
    uint64_t timestamp;
} iio_gyro_data_t;

int iio_gyro_init(void);
int iio_gyro_probe(iio_gyro_device_t *dev);
int iio_gyro_read_data(iio_gyro_device_t *dev, iio_gyro_data_t *data);
int iio_gyro_set_full_scale(iio_gyro_device_t *dev, uint32_t full_scale);
int iio_gyro_calibrate(iio_gyro_device_t *dev);
int iio_gyro_enable_interrupt(iio_gyro_device_t *dev, uint8_t irq_type);

#endif
