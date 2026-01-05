#ifndef ALERT_STYLE_H
#define ALERT_STYLE_H

#include <stdint.h>

typedef struct
{
    uint32_t background_color;
    uint32_t text_color;
    uint32_t border_color;
    uint32_t border_width;
    const char *font_family;
    uint32_t font_size;
} alert_style_t;

typedef struct
{
    const char *style_name;
    alert_style_t style_config;
    uint32_t is_default;
} alert_style_preset_t;

int alert_style_init(void);
int alert_style_create(const char *style_name, alert_style_t *style);
int alert_style_apply(uint32_t alert_id, const char *style_name);
int alert_style_get(const char *style_name, alert_style_t *style);
int alert_style_delete(const char *style_name);
int alert_style_list_presets(alert_style_preset_t *presets, uint32_t *count);
int alert_style_set_default(const char *style_name);
int alert_style_reset_to_default(uint32_t alert_id);

#endif
