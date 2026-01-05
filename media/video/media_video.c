#include "media_video.h"

int media_video_init(void)
{
    return 0;
}

int media_video_register_device(media_video_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int media_video_unregister_device(uint32_t device_id)
{
    return 0;
}

int media_video_capture_frame(uint32_t device_id, uint8_t *buffer, uint32_t *size)
{
    if (!buffer || !size)
    {
        return -1;
    }
    return 0;
}

int media_video_start_streaming(uint32_t device_id)
{
    return 0;
}

int media_video_stop_streaming(uint32_t device_id)
{
    return 0;
}
