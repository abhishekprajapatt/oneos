#ifndef AUDIO_CODEC_H
#define AUDIO_CODEC_H

#include <stdint.h>

typedef struct
{
    uint32_t codec_id;
    uint16_t vendor_id;
    uint16_t device_id;
    uint32_t sample_rate;
    uint8_t channels;
} audio_codec_device_t;

typedef struct
{
    uint8_t bits_per_sample;
    uint32_t sample_rate;
    uint8_t channels;
    uint32_t format;
} audio_codec_config_t;

int audio_codec_init(void);
int audio_codec_probe(audio_codec_device_t *dev);
int audio_codec_configure(audio_codec_device_t *dev, audio_codec_config_t *cfg);
int audio_codec_playback_start(audio_codec_device_t *dev);
int audio_codec_playback_stop(audio_codec_device_t *dev);
int audio_codec_set_volume(audio_codec_device_t *dev, uint8_t volume);

#endif
