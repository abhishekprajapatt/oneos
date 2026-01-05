#ifndef VAR_CACHE_FONTCONFIG_H
#define VAR_CACHE_FONTCONFIG_H

#include <stdint.h>

typedef struct
{
    uint32_t font_id;
    const char *font_name;
    const char *font_path;
    uint64_t cache_size;
    uint64_t last_accessed;
} var_cache_fontconfig_entry_t;

typedef struct
{
    uint32_t total_fonts;
    uint32_t cached_fonts;
    uint64_t total_cache_size;
} var_cache_fontconfig_state_t;

int var_cache_fontconfig_init(void);
int var_cache_fontconfig_list(var_cache_fontconfig_entry_t *entries, uint32_t *count);
int var_cache_fontconfig_get(const char *font_name, var_cache_fontconfig_entry_t *entry);
int var_cache_fontconfig_cache_font(const char *font_name, const char *font_path);
int var_cache_fontconfig_invalidate(const char *font_name);
int var_cache_fontconfig_rebuild(void);
int var_cache_fontconfig_get_state(var_cache_fontconfig_state_t *state);

#endif
