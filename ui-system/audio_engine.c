#include "audio_engine.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

AudioEngine *audio_init(void)
{
    AudioEngine *engine = (AudioEngine *)malloc(sizeof(AudioEngine));
    if (!engine)
        return NULL;

    memset(engine, 0, sizeof(AudioEngine));
    engine->channel_count = 0;
    engine->buffer_count = 0;
    engine->device_count = 0;
    engine->master_volume = 100;
    engine->active_device_id = 0;
    engine->total_samples_mixed = 0;
    engine->cpu_usage_percent = 0.0f;

    return engine;
}

void audio_cleanup(AudioEngine *engine)
{
    if (!engine)
        return;

    for (uint32_t i = 0; i < engine->channel_count; i++)
    {
        if (engine->channels[i].sample_buffer)
        {
            free(engine->channels[i].sample_buffer);
        }
    }

    for (uint32_t i = 0; i < engine->buffer_count; i++)
    {
        if (engine->buffers[i].data)
        {
            free(engine->buffers[i].data);
        }
    }

    memset(engine, 0, sizeof(AudioEngine));
    free(engine);
}

uint32_t audio_create_channel(AudioEngine *engine, uint32_t buffer_id)
{
    if (!engine || engine->channel_count >= MAX_AUDIO_CHANNELS)
        return 0;

    AudioBuffer *buffer = audio_get_buffer(engine, buffer_id);
    if (!buffer || !buffer->is_loaded)
        return 0;

    AudioChannel *channel = &engine->channels[engine->channel_count];
    channel->channel_id = engine->channel_count + 1;
    channel->state = CHANNEL_STATE_IDLE;
    channel->sample_buffer = (int16_t *)malloc(buffer->size);

    if (!channel->sample_buffer)
        return 0;

    memcpy(channel->sample_buffer, buffer->data, buffer->size);
    channel->buffer_size = buffer->size / sizeof(int16_t);
    channel->current_position = 0;
    channel->volume = 1.0f;
    channel->pan = 0.0f;
    channel->is_looping = false;
    channel->loop_start = 0;
    channel->loop_end = channel->buffer_size;
    channel->samples_played = 0;

    engine->channel_count++;
    return channel->channel_id;
}

int audio_destroy_channel(AudioEngine *engine, uint32_t channel_id)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->channel_count; i++)
    {
        if (engine->channels[i].channel_id == channel_id)
        {
            if (engine->channels[i].sample_buffer)
            {
                free(engine->channels[i].sample_buffer);
            }
            memmove(&engine->channels[i], &engine->channels[i + 1],
                    (engine->channel_count - i - 1) * sizeof(AudioChannel));
            engine->channel_count--;
            return 0;
        }
    }
    return -1;
}

int audio_play_channel(AudioEngine *engine, uint32_t channel_id)
{
    if (!engine)
        return -1;

    AudioChannel *channel = audio_get_channel(engine, channel_id);
    if (!channel || channel->state == CHANNEL_STATE_PLAYING)
        return -1;

    channel->state = CHANNEL_STATE_PLAYING;
    channel->current_position = 0;
    channel->samples_played = 0;
    return 0;
}

int audio_stop_channel(AudioEngine *engine, uint32_t channel_id)
{
    if (!engine)
        return -1;

    AudioChannel *channel = audio_get_channel(engine, channel_id);
    if (!channel)
        return -1;

    channel->state = CHANNEL_STATE_STOPPED;
    channel->current_position = 0;
    channel->samples_played = 0;
    return 0;
}

int audio_pause_channel(AudioEngine *engine, uint32_t channel_id)
{
    if (!engine)
        return -1;

    AudioChannel *channel = audio_get_channel(engine, channel_id);
    if (!channel || channel->state != CHANNEL_STATE_PLAYING)
        return -1;

    channel->state = CHANNEL_STATE_PAUSED;
    return 0;
}

int audio_resume_channel(AudioEngine *engine, uint32_t channel_id)
{
    if (!engine)
        return -1;

    AudioChannel *channel = audio_get_channel(engine, channel_id);
    if (!channel || channel->state != CHANNEL_STATE_PAUSED)
        return -1;

    channel->state = CHANNEL_STATE_PLAYING;
    return 0;
}

int audio_set_channel_volume(AudioEngine *engine, uint32_t channel_id, float volume)
{
    if (!engine || volume < 0.0f || volume > 1.0f)
        return -1;

    AudioChannel *channel = audio_get_channel(engine, channel_id);
    if (!channel)
        return -1;

    channel->volume = volume;
    return 0;
}

int audio_set_channel_pan(AudioEngine *engine, uint32_t channel_id, float pan)
{
    if (!engine || pan < -1.0f || pan > 1.0f)
        return -1;

    AudioChannel *channel = audio_get_channel(engine, channel_id);
    if (!channel)
        return -1;

    channel->pan = pan;
    return 0;
}

int audio_set_channel_loop(AudioEngine *engine, uint32_t channel_id,
                           bool loop, uint32_t start, uint32_t end)
{
    if (!engine)
        return -1;

    AudioChannel *channel = audio_get_channel(engine, channel_id);
    if (!channel || start >= end || end > channel->buffer_size)
        return -1;

    channel->is_looping = loop;
    channel->loop_start = start;
    channel->loop_end = end;
    return 0;
}

uint32_t audio_load_buffer(AudioEngine *engine, const uint8_t *data, uint32_t size,
                           AudioFormat format, uint32_t sample_rate, uint32_t channels)
{
    if (!engine || engine->buffer_count >= MAX_AUDIO_BUFFERS)
        return 0;
    if (!data || size == 0)
        return 0;

    AudioBuffer *buffer = &engine->buffers[engine->buffer_count];
    buffer->buffer_id = engine->buffer_count + 1;
    buffer->data = (uint8_t *)malloc(size);

    if (!buffer->data)
        return 0;

    memcpy(buffer->data, data, size);
    buffer->size = size;
    buffer->format = format;
    buffer->sample_rate = sample_rate;
    buffer->channels = channels;
    buffer->duration_ms = (size / (sample_rate * channels * sizeof(int16_t))) * 1000;
    buffer->is_loaded = true;

    engine->buffer_count++;
    return buffer->buffer_id;
}

int audio_unload_buffer(AudioEngine *engine, uint32_t buffer_id)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->buffer_count; i++)
    {
        if (engine->buffers[i].buffer_id == buffer_id)
        {
            if (engine->buffers[i].data)
            {
                free(engine->buffers[i].data);
            }
            memmove(&engine->buffers[i], &engine->buffers[i + 1],
                    (engine->buffer_count - i - 1) * sizeof(AudioBuffer));
            engine->buffer_count--;
            return 0;
        }
    }
    return -1;
}

int audio_update_buffer(AudioEngine *engine, uint32_t buffer_id,
                        const uint8_t *data, uint32_t size)
{
    if (!engine || !data || size == 0)
        return -1;

    AudioBuffer *buffer = audio_get_buffer(engine, buffer_id);
    if (!buffer)
        return -1;

    uint8_t *new_data = (uint8_t *)malloc(size);
    if (!new_data)
        return -1;

    if (buffer->data)
    {
        free(buffer->data);
    }

    memcpy(new_data, data, size);
    buffer->data = new_data;
    buffer->size = size;
    return 0;
}

uint32_t audio_register_device(AudioEngine *engine, AudioDeviceType type,
                               const char *name, uint32_t sample_rate, uint32_t channels)
{
    if (!engine || engine->device_count >= MAX_AUDIO_DEVICES || !name)
        return 0;

    AudioDevice *device = &engine->devices[engine->device_count];
    device->device_id = engine->device_count + 1;
    device->type = type;
    strncpy(device->name, name, 63);
    device->name[63] = '\0';
    device->sample_rate = sample_rate;
    device->channels = channels;
    device->format = AUDIO_FORMAT_PCM16;
    device->is_active = false;
    device->buffer_size = AUDIO_BUFFER_SIZE;

    engine->device_count++;
    return device->device_id;
}

int audio_unregister_device(AudioEngine *engine, uint32_t device_id)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->device_count; i++)
    {
        if (engine->devices[i].device_id == device_id)
        {
            if (engine->active_device_id == device_id)
            {
                engine->active_device_id = 0;
            }
            memmove(&engine->devices[i], &engine->devices[i + 1],
                    (engine->device_count - i - 1) * sizeof(AudioDevice));
            engine->device_count--;
            return 0;
        }
    }
    return -1;
}

int audio_set_active_device(AudioEngine *engine, uint32_t device_id)
{
    if (!engine)
        return -1;

    AudioDevice *device = audio_get_device(engine, device_id);
    if (!device)
        return -1;

    AudioDevice *prev = audio_get_device(engine, engine->active_device_id);
    if (prev)
        prev->is_active = false;

    device->is_active = true;
    engine->active_device_id = device_id;
    return 0;
}

int audio_mix_channels(AudioEngine *engine)
{
    if (!engine)
        return -1;

    for (uint32_t i = 0; i < engine->channel_count; i++)
    {
        AudioChannel *channel = &engine->channels[i];

        if (channel->state == CHANNEL_STATE_PLAYING)
        {
            if (channel->current_position < channel->buffer_size)
            {
                channel->current_position++;
                channel->samples_played++;
                engine->total_samples_mixed++;
            }
            else if (channel->is_looping)
            {
                channel->current_position = channel->loop_start;
            }
            else
            {
                channel->state = CHANNEL_STATE_STOPPED;
            }
        }
    }

    return 0;
}

int audio_process_audio(AudioEngine *engine, int16_t *output_buffer, uint32_t frame_count)
{
    if (!engine || !output_buffer || frame_count == 0)
        return -1;

    memset(output_buffer, 0, frame_count * sizeof(int16_t));

    int32_t mixed_sample = 0;
    for (uint32_t i = 0; i < frame_count; i++)
    {
        mixed_sample = 0;

        for (uint32_t j = 0; j < engine->channel_count; j++)
        {
            AudioChannel *channel = &engine->channels[j];

            if (channel->state == CHANNEL_STATE_PLAYING &&
                channel->current_position < channel->buffer_size)
            {

                int16_t sample = channel->sample_buffer[channel->current_position];
                int32_t scaled = (int32_t)(sample * channel->volume *
                                           (engine->master_volume / 100.0f));
                mixed_sample += scaled;
            }
        }

        if (mixed_sample > 32767)
            mixed_sample = 32767;
        else if (mixed_sample < -32768)
            mixed_sample = -32768;

        output_buffer[i] = (int16_t)mixed_sample;
    }

    audio_mix_channels(engine);
    return 0;
}

int audio_set_master_volume(AudioEngine *engine, uint32_t volume)
{
    if (!engine || volume > 100)
        return -1;

    engine->master_volume = volume;
    return 0;
}

uint32_t audio_get_master_volume(AudioEngine *engine)
{
    if (!engine)
        return 0;
    return engine->master_volume;
}

AudioChannel *audio_get_channel(AudioEngine *engine, uint32_t channel_id)
{
    if (!engine)
        return NULL;

    for (uint32_t i = 0; i < engine->channel_count; i++)
    {
        if (engine->channels[i].channel_id == channel_id)
        {
            return &engine->channels[i];
        }
    }
    return NULL;
}

AudioBuffer *audio_get_buffer(AudioEngine *engine, uint32_t buffer_id)
{
    if (!engine)
        return NULL;

    for (uint32_t i = 0; i < engine->buffer_count; i++)
    {
        if (engine->buffers[i].buffer_id == buffer_id)
        {
            return &engine->buffers[i];
        }
    }
    return NULL;
}

AudioDevice *audio_get_device(AudioEngine *engine, uint32_t device_id)
{
    if (!engine)
        return NULL;

    for (uint32_t i = 0; i < engine->device_count; i++)
    {
        if (engine->devices[i].device_id == device_id)
        {
            return &engine->devices[i];
        }
    }
    return NULL;
}

uint64_t audio_get_total_samples_mixed(AudioEngine *engine)
{
    if (!engine)
        return 0;
    return engine->total_samples_mixed;
}

float audio_get_cpu_usage(AudioEngine *engine)
{
    if (!engine)
        return 0.0f;
    return engine->cpu_usage_percent;
}
