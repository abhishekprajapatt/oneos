#ifndef UI_THEME_H
#define UI_THEME_H

#include <stdint.h>

typedef struct
{
    uint32_t primary_color;
    uint32_t secondary_color;
    uint32_t accent_color;
    uint32_t background_color;
    uint32_t text_color;
} ui_theme_palette_t;

typedef struct
{
    const char *theme_name;
    const char *font_family;
    uint32_t font_size;
    ui_theme_palette_t palette;
    const char *style_sheet;
} ui_theme_t;

typedef struct
{
    uint32_t total_themes;
    uint32_t active_theme;
    uint32_t custom_themes;
} ui_theme_stats_t;

int ui_theme_init(void);
int ui_theme_load(const char *theme_name, ui_theme_t *theme);
int ui_theme_save(const char *theme_name, ui_theme_t *theme);
int ui_theme_apply(const char *theme_name);
int ui_theme_create_custom(const char *theme_name, ui_theme_t *theme);
int ui_theme_delete(const char *theme_name);
int ui_theme_list_themes(char **theme_names, uint32_t *count);
int ui_theme_get_stats(ui_theme_stats_t *stats);

#endif
