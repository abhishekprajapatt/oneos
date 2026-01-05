#ifndef SYSTEM_PREFERENCES_H
#define SYSTEM_PREFERENCES_H

#include <stdint.h>

typedef struct
{
    const char *preference_category;
    const char *preference_key;
    const char *preference_value;
} preference_t;

int system_preferences_init(void);
int system_preferences_get(const char *category, const char *key, char *value, uint32_t max_len);
int system_preferences_set(const char *category, const char *key, const char *value);
int system_preferences_reset_category(const char *category);
int system_preferences_list_categories(char **categories, uint32_t *count);
int system_preferences_export(const char *output_path);
int system_preferences_import(const char *input_path);
int system_preferences_sync_with_cloud(void);

#endif
