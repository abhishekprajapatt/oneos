#ifndef AUDIO_DSP_H
#define AUDIO_DSP_H

#include <stdint.h>

typedef struct
{
    uint32_t dsp_id;
    uint32_t base_address;
    uint32_t fw_version;
    uint32_t flags;
} audio_dsp_device_t;

typedef struct
{
    uint8_t *coeff_data;
    uint32_t coeff_len;
    uint8_t filter_type;
} audio_dsp_filter_t;

int audio_dsp_init(void);
int audio_dsp_probe(audio_dsp_device_t *dev);
int audio_dsp_load_firmware(audio_dsp_device_t *dev, uint8_t *fw, uint32_t fw_size);
int audio_dsp_configure_filter(audio_dsp_device_t *dev, audio_dsp_filter_t *filter);
int audio_dsp_process_stream(audio_dsp_device_t *dev, uint8_t *input, uint8_t *output, uint32_t len);
int audio_dsp_set_gain(audio_dsp_device_t *dev, uint8_t gain);

#endif
