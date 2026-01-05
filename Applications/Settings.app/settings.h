#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdint.h>

typedef struct
{
    const char *setting_key;
    const char *setting_value;
    const char *setting_type;
} setting_item_t;

int settings_init(void);
int settings_get(const char *key, char *value, uint32_t max_len);
int settings_set(const char *key, const char *value);
int settings_reset_to_default(void);
int settings_export_backup(const char *output_path);
int settings_import_backup(const char *input_path);
int settings_list_all_categories(char **categories, uint32_t *count);
int settings_get_category_settings(const char *category, setting_item_t *items, uint32_t *count);

#endif
