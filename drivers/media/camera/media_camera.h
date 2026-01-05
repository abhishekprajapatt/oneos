#ifndef MEDIA_CAMERA_H
#define MEDIA_CAMERA_H

#include <stdint.h>

typedef struct
{
    uint32_t camera_id;
    uint32_t base_address;
    uint16_t sensor_width;
    uint16_t sensor_height;
    uint32_t flags;
} media_camera_device_t;

typedef struct
{
    uint32_t format;
    uint16_t width;
    uint16_t height;
    uint32_t fps;
} media_camera_format_t;

int media_camera_init(void);
int media_camera_probe(media_camera_device_t *dev);
int media_camera_set_format(media_camera_device_t *dev, media_camera_format_t *fmt);
int media_camera_start_stream(media_camera_device_t *dev);
int media_camera_stop_stream(media_camera_device_t *dev);
int media_camera_capture_frame(media_camera_device_t *dev, uint8_t *buffer, uint32_t buf_size);

#endif
