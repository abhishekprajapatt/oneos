#ifndef TOAST_STYLE_H
#define TOAST_STYLE_H

#include <stdint.h>

typedef struct
{
    uint32_t background_color;
    uint32_t text_color;
    const char *animation_style;
    uint32_t corner_radius;
    const char *font_family;
    uint32_t font_size;
} toast_style_t;

typedef struct
{
    const char *style_name;
    toast_style_t style_config;
    uint32_t is_default;
} toast_style_preset_t;

int toast_style_init(void);
int toast_style_create(const char *style_name, toast_style_t *style);
int toast_style_apply(uint32_t toast_id, const char *style_name);
int toast_style_get(const char *style_name, toast_style_t *style);
int toast_style_delete(const char *style_name);
int toast_style_list_presets(toast_style_preset_t *presets, uint32_t *count);
int toast_style_set_default(const char *style_name);
int toast_style_reset_to_default(uint32_t toast_id);

#endif
