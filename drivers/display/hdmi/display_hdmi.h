#ifndef DISPLAY_HDMI_H
#define DISPLAY_HDMI_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t base_address;
    uint8_t port;
    uint32_t flags;
} display_hdmi_device_t;

typedef struct
{
    uint32_t resolution;
    uint8_t color_depth;
    uint8_t color_space;
    uint32_t refresh_rate;
} display_hdmi_mode_t;

int display_hdmi_init(void);
int display_hdmi_probe(display_hdmi_device_t *dev);
int display_hdmi_set_mode(display_hdmi_device_t *dev, display_hdmi_mode_t *mode);
int display_hdmi_detect_connected(display_hdmi_device_t *dev);
int display_hdmi_enable_audio(display_hdmi_device_t *dev);
int display_hdmi_send_infoframe(display_hdmi_device_t *dev, uint8_t *frame, uint32_t len);

#endif
