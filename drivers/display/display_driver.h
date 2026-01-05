#ifndef DISPLAY_DRIVER_H
#define DISPLAY_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    uint32_t bits_per_pixel;
    uint32_t refresh_rate;
} display_mode_t;

typedef struct
{
    uint32_t device_id;
    display_mode_t mode;
    void *framebuffer;
} display_device_t;

int display_init(void);
int display_open(display_device_t *dev);
int display_close(display_device_t *dev);
int display_set_mode(display_device_t *dev, display_mode_t *mode);
int display_get_framebuffer(display_device_t *dev, void **buffer);
int display_update(display_device_t *dev);

#endif
