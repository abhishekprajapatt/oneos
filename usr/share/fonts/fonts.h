#ifndef FONTS_H
#define FONTS_H

#include <stdint.h>

typedef struct
{
    const char *font_name;
    const char *font_file;
    const char *font_family;
    uint32_t font_weight;
    uint32_t is_scalable;
} font_entry_t;

typedef struct
{
    uint32_t total_fonts;
    uint32_t scalable_fonts;
    uint32_t bitmap_fonts;
    uint64_t total_size;
} font_stats_t;

int fonts_init(void);
int fonts_list_fonts(font_entry_t *fonts, uint32_t *count);
int fonts_get_font(const char *font_name, font_entry_t *font);
int fonts_search_by_family(const char *family, font_entry_t *fonts, uint32_t *count);
int fonts_install_font(const char *font_path);
int fonts_get_stats(font_stats_t *stats);

#endif
