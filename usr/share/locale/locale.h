#ifndef LOCALE_H
#define LOCALE_H

#include <stdint.h>

typedef struct
{
    const char *locale_code;
    const char *language;
    const char *country;
    const char *encoding;
} locale_entry_t;

typedef struct
{
    uint32_t total_locales;
    uint32_t languages_supported;
    uint32_t character_sets;
} locale_stats_t;

int locale_init(void);
int locale_list_locales(locale_entry_t *locales, uint32_t *count);
int locale_get_locale(const char *locale_code, locale_entry_t *locale);
int locale_set_default_locale(const char *locale_code);
int locale_get_current_locale(char *locale_code, uint32_t max_len);
int locale_translate_string(const char *domain, const char *string, char *translation, uint32_t max_len);
int locale_get_stats(locale_stats_t *stats);

#endif
