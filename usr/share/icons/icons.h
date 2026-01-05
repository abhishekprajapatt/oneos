#ifndef ICONS_H
#define ICONS_H

#include <stdint.h>

typedef struct
{
    const char *icon_name;
    const char *icon_file;
    const char *icon_theme;
    uint32_t icon_size;
    const char *icon_type;
} icon_entry_t;

typedef struct
{
    uint32_t total_icons;
    uint32_t themes_available;
    uint32_t icon_sizes;
    uint64_t total_size;
} icon_stats_t;

int icons_init(void);
int icons_list_icons(icon_entry_t *icons, uint32_t *count);
int icons_get_icon(const char *icon_name, icon_entry_t *icon);
int icons_search_by_theme(const char *theme, icon_entry_t *icons, uint32_t *count);
int icons_get_icon_by_size(const char *icon_name, uint32_t size, icon_entry_t *icon);
int icons_get_stats(icon_stats_t *stats);

#endif
