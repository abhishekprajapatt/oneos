#ifndef UI_CORE_H
#define UI_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t window_id;
    const char *window_title;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t is_visible;
} ui_window_t;

typedef struct
{
    uint32_t total_windows;
    uint32_t active_windows;
    uint32_t total_widgets;
    uint64_t memory_usage;
} ui_core_stats_t;

int ui_core_init(void);
int ui_core_create_window(const char *title, uint32_t width, uint32_t height, ui_window_t *window);
int ui_core_destroy_window(uint32_t window_id);
int ui_core_show_window(uint32_t window_id);
int ui_core_hide_window(uint32_t window_id);
int ui_core_set_window_properties(uint32_t window_id, uint32_t x, uint32_t y, uint32_t width, uint32_t height);
int ui_core_list_windows(ui_window_t *windows, uint32_t *count);
int ui_core_get_stats(ui_core_stats_t *stats);
int ui_core_cleanup(void);

#endif
