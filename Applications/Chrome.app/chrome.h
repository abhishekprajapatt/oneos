#ifndef CHROME_H
#define CHROME_H

#include <stdint.h>

typedef struct
{
    uint32_t tab_id;
    const char *tab_title;
    const char *tab_url;
    uint32_t is_active;
    uint32_t load_progress;
} browser_tab_t;

typedef struct
{
    uint32_t total_tabs;
    uint32_t active_tab;
    uint64_t memory_usage;
    uint32_t total_history;
} browser_stats_t;

int chrome_init(void);
int chrome_create_tab(const char *url, uint32_t *tab_id);
int chrome_close_tab(uint32_t tab_id);
int chrome_navigate(uint32_t tab_id, const char *url);
int chrome_go_back(uint32_t tab_id);
int chrome_go_forward(uint32_t tab_id);
int chrome_reload_tab(uint32_t tab_id);
int chrome_set_active_tab(uint32_t tab_id);
int chrome_list_tabs(browser_tab_t *tabs, uint32_t *count);
int chrome_get_stats(browser_stats_t *stats);

#endif
