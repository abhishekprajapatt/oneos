#ifndef MEDIA_AUDIO_H
#define MEDIA_AUDIO_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    uint32_t sample_rate;
    uint32_t channels;
    uint32_t bit_depth;
} media_audio_device_t;

typedef struct
{
    uint32_t device_count;
    uint32_t active_devices;
    uint64_t samples_processed;
} media_audio_state_t;

int media_audio_init(void);
int media_audio_register_device(media_audio_device_t *device);
int media_audio_unregister_device(uint32_t device_id);
int media_audio_play(uint32_t device_id, uint8_t *buffer, uint32_t size);
int media_audio_record(uint32_t device_id, uint8_t *buffer, uint32_t *size);
int media_audio_set_volume(uint32_t device_id, uint32_t volume);

#endif
