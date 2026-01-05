#ifndef IIO_ADC_H
#define IIO_ADC_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint32_t resolution;
    uint8_t channel_count;
    uint32_t sample_rate;
} iio_adc_device_t;

typedef struct
{
    uint32_t *channel_data;
    uint32_t num_channels;
    uint64_t timestamp;
    uint32_t flags;
} iio_adc_sample_t;

int iio_adc_init(void);
int iio_adc_probe(iio_adc_device_t *dev);
int iio_adc_enable_channel(iio_adc_device_t *dev, uint8_t channel);
int iio_adc_disable_channel(iio_adc_device_t *dev, uint8_t channel);
int iio_adc_read_sample(iio_adc_device_t *dev, iio_adc_sample_t *sample);
int iio_adc_set_sample_rate(iio_adc_device_t *dev, uint32_t rate);

#endif
