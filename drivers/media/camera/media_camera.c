#include "media_camera.h"

int media_camera_init(void)
{
    return 0;
}

int media_camera_probe(media_camera_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int media_camera_set_format(media_camera_device_t *dev, media_camera_format_t *fmt)
{
    if (!dev || !fmt)
    {
        return -1;
    }
    return 0;
}

int media_camera_start_stream(media_camera_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int media_camera_stop_stream(media_camera_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int media_camera_capture_frame(media_camera_device_t *dev, uint8_t *buffer, uint32_t buf_size)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    return 0;
}
