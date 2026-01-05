#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <stdint.h>

typedef struct
{
    uint32_t widget_id;
    const char *widget_type;
    uint32_t parent_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    uint32_t is_enabled;
} ui_widget_t;

typedef struct
{
    uint32_t total_widgets;
    uint32_t widget_types;
    uint32_t enabled_widgets;
    uint32_t disabled_widgets;
} ui_widget_stats_t;

int ui_widget_init(void);
int ui_widget_create(const char *type, uint32_t parent_id, ui_widget_t *widget);
int ui_widget_destroy(uint32_t widget_id);
int ui_widget_set_position(uint32_t widget_id, uint32_t x, uint32_t y);
int ui_widget_set_size(uint32_t widget_id, uint32_t width, uint32_t height);
int ui_widget_enable(uint32_t widget_id);
int ui_widget_disable(uint32_t widget_id);
int ui_widget_list_by_parent(uint32_t parent_id, ui_widget_t *widgets, uint32_t *count);
int ui_widget_get_stats(ui_widget_stats_t *stats);

#endif
