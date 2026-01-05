#ifndef SAFARI_H
#define SAFARI_H

#include <stdint.h>

typedef struct
{
    uint32_t tab_id;
    const char *tab_title;
    const char *tab_url;
    uint32_t is_active;
    uint32_t load_progress;
} safari_tab_t;

typedef struct
{
    uint32_t total_tabs;
    uint32_t reading_list_items;
    uint32_t bookmarks;
} safari_stats_t;

int safari_init(void);
int safari_create_tab(const char *url, uint32_t *tab_id);
int safari_close_tab(uint32_t tab_id);
int safari_navigate(uint32_t tab_id, const char *url);
int safari_go_back(uint32_t tab_id);
int safari_go_forward(uint32_t tab_id);
int safari_add_bookmark(const char *url, const char *title);
int safari_add_to_reading_list(const char *url);
int safari_list_tabs(safari_tab_t *tabs, uint32_t *count);
int safari_get_stats(safari_stats_t *stats);

#endif
