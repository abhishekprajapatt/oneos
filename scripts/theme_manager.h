#ifndef THEME_MANAGER_H
#define THEME_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *theme_name;
    uint32_t color_scheme;
    uint32_t accent_color;
} theme_config_t;

int theme_manager_init(void);
int theme_manager_load_theme(const char *theme_path);
int theme_manager_apply_theme(theme_config_t *theme);
int theme_manager_list_themes(char **themes, uint32_t max_themes);
int theme_manager_create_custom(const char *theme_name, theme_config_t *config);
int theme_manager_export_theme(const char *theme_name, const char *output_path);

#endif
