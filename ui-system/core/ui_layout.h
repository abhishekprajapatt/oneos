#ifndef UI_LAYOUT_H
#define UI_LAYOUT_H

#include <stdint.h>

typedef struct
{
    const char *layout_type;
    uint32_t spacing;
    uint32_t padding;
    const char *alignment;
} ui_layout_config_t;

typedef struct
{
    uint32_t layout_id;
    uint32_t parent_id;
    ui_layout_config_t config;
    uint32_t child_count;
} ui_layout_t;

typedef struct
{
    uint32_t total_layouts;
    uint32_t active_layouts;
    uint32_t layout_types;
} ui_layout_stats_t;

int ui_layout_init(void);
int ui_layout_create(uint32_t parent_id, ui_layout_config_t *config, ui_layout_t *layout);
int ui_layout_destroy(uint32_t layout_id);
int ui_layout_add_child(uint32_t layout_id, uint32_t child_id);
int ui_layout_remove_child(uint32_t layout_id, uint32_t child_id);
int ui_layout_recalculate(uint32_t layout_id);
int ui_layout_set_config(uint32_t layout_id, ui_layout_config_t *config);
int ui_layout_get_stats(ui_layout_stats_t *stats);

#endif
