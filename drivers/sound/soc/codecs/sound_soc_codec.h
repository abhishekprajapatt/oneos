#ifndef SOUND_SOC_CODECS_H
#define SOUND_SOC_CODECS_H

#include <stdint.h>

typedef struct
{
    uint32_t codec_id;
    uint8_t address;
    uint32_t sample_rate;
    uint8_t data_format;
} sound_soc_codec_device_t;

typedef struct
{
    uint32_t sample_rate;
    uint8_t bits_per_sample;
    uint8_t channels;
    uint8_t dai_format;
} sound_soc_codec_config_t;

int sound_soc_codec_init(void);
int sound_soc_codec_probe(sound_soc_codec_device_t *dev);
int sound_soc_codec_configure(sound_soc_codec_device_t *dev, sound_soc_codec_config_t *cfg);
int sound_soc_codec_set_volume(sound_soc_codec_device_t *dev, uint8_t volume);
int sound_soc_codec_mute(sound_soc_codec_device_t *dev, uint8_t mute);
int sound_soc_codec_set_eq(sound_soc_codec_device_t *dev, uint8_t *eq_data, uint32_t len);

#endif
