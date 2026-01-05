#include "sound_soc_codec.h"

int sound_soc_codec_init(void)
{
    return 0;
}

int sound_soc_codec_probe(sound_soc_codec_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_soc_codec_configure(sound_soc_codec_device_t *dev, sound_soc_codec_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int sound_soc_codec_set_volume(sound_soc_codec_device_t *dev, uint8_t volume)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_soc_codec_mute(sound_soc_codec_device_t *dev, uint8_t mute)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int sound_soc_codec_set_eq(sound_soc_codec_device_t *dev, uint8_t *eq_data, uint32_t len)
{
    if (!dev || !eq_data)
    {
        return -1;
    }
    return 0;
}
