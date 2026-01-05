#include "sound.h"

int sound_init(void)
{
    return 0;
}

int sound_list_devices(audio_device_t *devices, uint32_t *count)
{
    if (!devices || !count)
    {
        return -1;
    }
    return 0;
}

int sound_get_device(uint32_t device_id, audio_device_t *device)
{
    if (device_id == 0 || !device)
    {
        return -1;
    }
    return 0;
}

int sound_set_volume(uint32_t device_id, uint32_t volume)
{
    if (device_id == 0 || volume > 100)
    {
        return -1;
    }
    return 0;
}

int sound_get_volume(uint32_t device_id, uint32_t *volume)
{
    if (device_id == 0 || !volume)
    {
        return -1;
    }
    return 0;
}

int sound_mute_device(uint32_t device_id)
{
    if (device_id == 0)
    {
        return -1;
    }
    return 0;
}

int sound_unmute_device(uint32_t device_id)
{
    if (device_id == 0)
    {
        return -1;
    }
    return 0;
}

int sound_set_default_device(uint32_t device_id)
{
    if (device_id == 0)
    {
        return -1;
    }
    return 0;
}

int sound_get_stats(sound_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
