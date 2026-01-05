#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdint.h>

typedef struct
{
    uint32_t display_id;
    uint32_t width;
    uint32_t height;
    uint32_t refresh_rate;
    const char *color_format;
    uint32_t brightness;
    uint32_t contrast;
} display_settings_t;

typedef struct
{
    uint32_t total_displays;
    uint32_t active_displays;
    uint32_t inactive_displays;
} display_stats_t;

int display_init(void);
int display_list_connected(display_settings_t *displays, uint32_t *count);
int display_get_current(display_settings_t *settings);
int display_set_resolution(uint32_t display_id, uint32_t width, uint32_t height);
int display_set_refresh_rate(uint32_t display_id, uint32_t refresh_rate);
int display_set_brightness(uint32_t display_id, uint32_t brightness);
int display_set_contrast(uint32_t display_id, uint32_t contrast);
int display_get_stats(display_stats_t *stats);

#endif
