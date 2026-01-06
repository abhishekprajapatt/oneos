#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

typedef struct
{
    const char *name;
    const char *path;
    uint32_t size;
} font_info_t;

int fonts_init(void);
int fonts_load_font(const char *font_path);
int fonts_list_fonts(font_info_t **fonts, uint32_t *count);
int fonts_get_font_info(const char *font_name, font_info_t *info);
int fonts_unload_font(const char *font_name);
int fonts_set_default_font(const char *font_name);

#endif
