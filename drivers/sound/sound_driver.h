#ifndef SOUND_DRIVER_H
#define SOUND_DRIVER_H

#include <stdint.h>

#define SOUND_PCM_PLAYBACK 0
#define SOUND_PCM_CAPTURE 1

typedef struct
{
    uint32_t rate;
    uint8_t channels;
    uint8_t bits;
    uint32_t period_size;
} sound_config_t;

typedef struct
{
    uint32_t device_id;
    sound_config_t config;
    uint32_t direction;
} sound_pcm_t;

int sound_init(void);
int sound_open_pcm(sound_pcm_t *pcm);
int sound_close_pcm(sound_pcm_t *pcm);
int sound_write_pcm(sound_pcm_t *pcm, void *buffer, uint32_t size);
int sound_read_pcm(sound_pcm_t *pcm, void *buffer, uint32_t size);

#endif
