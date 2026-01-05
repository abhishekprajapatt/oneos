#ifndef TASKBAR_H
#define TASKBAR_H

#include <stdint.h>

typedef struct
{
    uint32_t taskbar_id;
    uint32_t window_id;
    const char *window_title;
    const char *app_icon;
    uint32_t is_active;
} taskbar_item_t;

typedef struct
{
    uint32_t taskbar_position;
    uint32_t auto_hide;
    uint32_t show_clock;
    uint32_t show_system_tray;
} taskbar_settings_t;

typedef struct
{
    uint32_t total_items;
    uint32_t active_items;
    uint32_t pinned_items;
    uint32_t running_apps;
} taskbar_stats_t;

int taskbar_init(void);
int taskbar_list_items(taskbar_item_t *items, uint32_t *count);
int taskbar_add_item(uint32_t window_id, taskbar_item_t *item);
int taskbar_remove_item(uint32_t item_id);
int taskbar_activate_item(uint32_t item_id);
int taskbar_pin_item(uint32_t item_id);
int taskbar_unpin_item(uint32_t item_id);
int taskbar_set_position(uint32_t position);
int taskbar_toggle_auto_hide(uint32_t enable);
int taskbar_get_stats(taskbar_stats_t *stats);

#endif
