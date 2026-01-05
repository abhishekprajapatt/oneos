#include "audio_driver.h"

int audio_init(void)
{
    return 0;
}

int audio_open_stream(audio_stream_t *stream)
{
    if (!stream)
    {
        return -1;
    }
    return 0;
}

int audio_close_stream(audio_stream_t *stream)
{
    if (!stream)
    {
        return -1;
    }
    return 0;
}

int audio_write(audio_stream_t *stream, void *data, uint32_t size)
{
    if (!stream || !data || size == 0)
    {
        return -1;
    }
    return 0;
}

int audio_read(audio_stream_t *stream, void *data, uint32_t size)
{
    if (!stream || !data || size == 0)
    {
        return -1;
    }
    return 0;
}
