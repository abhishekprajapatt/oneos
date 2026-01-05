#ifndef LIBRARY_FONTS_H
#define LIBRARY_FONTS_H

#include <stdint.h>

typedef struct
{
    uint32_t font_id;
    const char *font_name;
    const char *font_family;
    uint32_t font_size;
    uint32_t font_weight;
} library_fonts_entry_t;

typedef struct
{
    uint32_t total_fonts;
    uint32_t system_fonts;
    uint32_t user_fonts;
    uint64_t total_font_size;
} library_fonts_state_t;

int library_fonts_init(void);
int library_fonts_register(library_fonts_entry_t *entry);
int library_fonts_unregister(uint32_t font_id);
int library_fonts_get_font(const char *name, library_fonts_entry_t *entry);
int library_fonts_list_family(const char *family, library_fonts_entry_t *results, uint32_t *count);
int library_fonts_get_state(library_fonts_state_t *state);

#endif
