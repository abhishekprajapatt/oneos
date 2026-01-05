#ifndef MEDIA_VIDEO_H
#define MEDIA_VIDEO_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    uint32_t resolution_width;
    uint32_t resolution_height;
    uint32_t refresh_rate;
} media_video_device_t;

typedef struct
{
    uint32_t device_count;
    uint32_t active_streams;
    uint64_t frames_processed;
} media_video_state_t;

int media_video_init(void);
int media_video_register_device(media_video_device_t *device);
int media_video_unregister_device(uint32_t device_id);
int media_video_capture_frame(uint32_t device_id, uint8_t *buffer, uint32_t *size);
int media_video_start_streaming(uint32_t device_id);
int media_video_stop_streaming(uint32_t device_id);

#endif
