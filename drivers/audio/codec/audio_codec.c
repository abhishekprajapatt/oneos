#include "audio_codec.h"

int audio_codec_init(void)
{
    return 0;
}

int audio_codec_probe(audio_codec_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int audio_codec_configure(audio_codec_device_t *dev, audio_codec_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int audio_codec_playback_start(audio_codec_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int audio_codec_playback_stop(audio_codec_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int audio_codec_set_volume(audio_codec_device_t *dev, uint8_t volume)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
