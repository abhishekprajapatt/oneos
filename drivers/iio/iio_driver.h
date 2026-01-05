#ifndef IIO_DRIVER_H
#define IIO_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t scan_index;
    uint32_t type;
} iio_channel_t;

typedef struct
{
    uint32_t device_id;
    iio_channel_t *channels;
    uint32_t channel_count;
} iio_device_t;

int iio_init(void);
int iio_register_device(iio_device_t *dev);
int iio_read_channel(iio_device_t *dev, uint32_t channel, int32_t *value);
int iio_enable_channel(iio_device_t *dev, uint32_t channel);
int iio_disable_channel(iio_device_t *dev, uint32_t channel);

#endif
