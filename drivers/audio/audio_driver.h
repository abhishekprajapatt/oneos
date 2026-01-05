#ifndef AUDIO_DRIVER_H
#define AUDIO_DRIVER_H

#include <stdint.h>

#define AUDIO_SAMPLE_RATE_48KHZ 48000
#define AUDIO_SAMPLE_RATE_44KHZ 44100
#define AUDIO_CHANNELS_MONO 1
#define AUDIO_CHANNELS_STEREO 2

typedef struct
{
    uint32_t sample_rate;
    uint8_t channels;
    uint8_t bits_per_sample;
    uint32_t buffer_size;
} audio_config_t;

typedef struct
{
    uint32_t device_id;
    audio_config_t config;
    void *buffer;
} audio_stream_t;

int audio_init(void);
int audio_open_stream(audio_stream_t *stream);
int audio_close_stream(audio_stream_t *stream);
int audio_write(audio_stream_t *stream, void *data, uint32_t size);
int audio_read(audio_stream_t *stream, void *data, uint32_t size);

#endif
