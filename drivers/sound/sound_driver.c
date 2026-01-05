#include "sound_driver.h"

int sound_init(void)
{
    return 0;
}

int sound_open_pcm(sound_pcm_t *pcm)
{
    if (!pcm)
    {
        return -1;
    }
    return 0;
}

int sound_close_pcm(sound_pcm_t *pcm)
{
    if (!pcm)
    {
        return -1;
    }
    return 0;
}

int sound_write_pcm(sound_pcm_t *pcm, void *buffer, uint32_t size)
{
    if (!pcm || !buffer || size == 0)
    {
        return -1;
    }
    return 0;
}

int sound_read_pcm(sound_pcm_t *pcm, void *buffer, uint32_t size)
{
    if (!pcm || !buffer || size == 0)
    {
        return -1;
    }
    return 0;
}
