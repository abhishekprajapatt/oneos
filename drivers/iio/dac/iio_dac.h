#ifndef IIO_DAC_H
#define IIO_DAC_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint32_t resolution;
    uint8_t channel_count;
    uint32_t update_rate;
} iio_dac_device_t;

typedef struct
{
    uint32_t *channel_data;
    uint32_t num_channels;
    uint32_t flags;
} iio_dac_output_t;

int iio_dac_init(void);
int iio_dac_probe(iio_dac_device_t *dev);
int iio_dac_enable_channel(iio_dac_device_t *dev, uint8_t channel);
int iio_dac_disable_channel(iio_dac_device_t *dev, uint8_t channel);
int iio_dac_write_output(iio_dac_device_t *dev, iio_dac_output_t *output);
int iio_dac_set_update_rate(iio_dac_device_t *dev, uint32_t rate);

#endif
