#include "media_audio.h"

int media_audio_init(void)
{
    return 0;
}

int media_audio_register_device(media_audio_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int media_audio_unregister_device(uint32_t device_id)
{
    return 0;
}

int media_audio_play(uint32_t device_id, uint8_t *buffer, uint32_t size)
{
    if (!buffer)
    {
        return -1;
    }
    return 0;
}

int media_audio_record(uint32_t device_id, uint8_t *buffer, uint32_t *size)
{
    if (!buffer || !size)
    {
        return -1;
    }
    return 0;
}

int media_audio_set_volume(uint32_t device_id, uint32_t volume)
{
    return 0;
}
