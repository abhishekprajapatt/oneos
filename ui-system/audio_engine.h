#ifndef AUDIO_ENGINE_H
#define AUDIO_ENGINE_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_AUDIO_CHANNELS 256
#define MAX_AUDIO_BUFFERS 1024
#define MAX_AUDIO_DEVICES 32
#define AUDIO_BUFFER_SIZE 4096

typedef enum
{
    AUDIO_FORMAT_PCM16 = 0,
    AUDIO_FORMAT_PCM32 = 1,
    AUDIO_FORMAT_FLOAT32 = 2,
    AUDIO_FORMAT_ADPCM = 3
} AudioFormat;

typedef enum
{
    AUDIO_DEVICE_OUTPUT = 0,
    AUDIO_DEVICE_INPUT = 1,
    AUDIO_DEVICE_SPEAKER = 2,
    AUDIO_DEVICE_MICROPHONE = 3,
    AUDIO_DEVICE_HEADPHONE = 4
} AudioDeviceType;

typedef enum
{
    CHANNEL_STATE_IDLE = 0,
    CHANNEL_STATE_PLAYING = 1,
    CHANNEL_STATE_PAUSED = 2,
    CHANNEL_STATE_STOPPED = 3
} ChannelState;

typedef struct
{
    uint32_t channel_id;
    ChannelState state;
    int16_t *sample_buffer;
    uint32_t buffer_size;
    uint32_t current_position;
    float volume;
    float pan;
    bool is_looping;
    uint32_t loop_start;
    uint32_t loop_end;
    uint64_t samples_played;
} AudioChannel;

typedef struct
{
    uint32_t buffer_id;
    uint8_t *data;
    uint32_t size;
    AudioFormat format;
    uint32_t sample_rate;
    uint32_t channels;
    uint32_t duration_ms;
    bool is_loaded;
} AudioBuffer;

typedef struct
{
    uint32_t device_id;
    AudioDeviceType type;
    char name[64];
    uint32_t sample_rate;
    uint32_t channels;
    AudioFormat format;
    bool is_active;
    uint32_t buffer_size;
} AudioDevice;

typedef struct
{
    AudioChannel channels[MAX_AUDIO_CHANNELS];
    uint32_t channel_count;
    AudioBuffer buffers[MAX_AUDIO_BUFFERS];
    uint32_t buffer_count;
    AudioDevice devices[MAX_AUDIO_DEVICES];
    uint32_t device_count;
    uint32_t master_volume;
    uint32_t active_device_id;
    uint64_t total_samples_mixed;
    float cpu_usage_percent;
} AudioEngine;

AudioEngine *audio_init(void);
void audio_cleanup(AudioEngine *engine);

uint32_t audio_create_channel(AudioEngine *engine, uint32_t buffer_id);
int audio_destroy_channel(AudioEngine *engine, uint32_t channel_id);
int audio_play_channel(AudioEngine *engine, uint32_t channel_id);
int audio_stop_channel(AudioEngine *engine, uint32_t channel_id);
int audio_pause_channel(AudioEngine *engine, uint32_t channel_id);
int audio_resume_channel(AudioEngine *engine, uint32_t channel_id);

int audio_set_channel_volume(AudioEngine *engine, uint32_t channel_id, float volume);
int audio_set_channel_pan(AudioEngine *engine, uint32_t channel_id, float pan);
int audio_set_channel_loop(AudioEngine *engine, uint32_t channel_id,
                           bool loop, uint32_t start, uint32_t end);

uint32_t audio_load_buffer(AudioEngine *engine, const uint8_t *data, uint32_t size,
                           AudioFormat format, uint32_t sample_rate, uint32_t channels);
int audio_unload_buffer(AudioEngine *engine, uint32_t buffer_id);
int audio_update_buffer(AudioEngine *engine, uint32_t buffer_id,
                        const uint8_t *data, uint32_t size);

uint32_t audio_register_device(AudioEngine *engine, AudioDeviceType type,
                               const char *name, uint32_t sample_rate, uint32_t channels);
int audio_unregister_device(AudioEngine *engine, uint32_t device_id);
int audio_set_active_device(AudioEngine *engine, uint32_t device_id);

int audio_mix_channels(AudioEngine *engine);
int audio_process_audio(AudioEngine *engine, int16_t *output_buffer, uint32_t frame_count);

int audio_set_master_volume(AudioEngine *engine, uint32_t volume);
uint32_t audio_get_master_volume(AudioEngine *engine);

AudioChannel *audio_get_channel(AudioEngine *engine, uint32_t channel_id);
AudioBuffer *audio_get_buffer(AudioEngine *engine, uint32_t buffer_id);
AudioDevice *audio_get_device(AudioEngine *engine, uint32_t device_id);

uint64_t audio_get_total_samples_mixed(AudioEngine *engine);
float audio_get_cpu_usage(AudioEngine *engine);

#endif
