#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    const char *device_type;
    uint32_t volume;
    uint32_t is_muted;
} audio_device_t;

typedef struct
{
    uint32_t master_volume;
    uint32_t is_muted;
    uint32_t total_devices;
    uint32_t active_device;
} sound_settings_t;

typedef struct
{
    uint32_t total_devices;
    uint32_t active_devices;
    uint32_t playback_devices;
    uint32_t recording_devices;
} sound_stats_t;

int sound_init(void);
int sound_list_devices(audio_device_t *devices, uint32_t *count);
int sound_get_device(uint32_t device_id, audio_device_t *device);
int sound_set_volume(uint32_t device_id, uint32_t volume);
int sound_get_volume(uint32_t device_id, uint32_t *volume);
int sound_mute_device(uint32_t device_id);
int sound_unmute_device(uint32_t device_id);
int sound_set_default_device(uint32_t device_id);
int sound_get_stats(sound_stats_t *stats);

#endif
