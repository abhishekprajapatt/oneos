#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include <stdint.h>

typedef struct
{
    uint32_t red;
    uint32_t green;
    uint32_t blue;
    uint32_t alpha;
} ui_color_t;

typedef struct
{
    uint32_t widget_id;
    uint32_t x;
    uint32_t y;
    uint32_t width;
    uint32_t height;
    ui_color_t background_color;
    ui_color_t foreground_color;
} ui_render_context_t;

typedef struct
{
    uint32_t total_renders;
    uint32_t successful_renders;
    uint32_t failed_renders;
    uint64_t total_render_time;
} ui_renderer_stats_t;

int ui_renderer_init(void);
int ui_renderer_draw_rect(ui_render_context_t *context);
int ui_renderer_draw_circle(uint32_t x, uint32_t y, uint32_t radius, ui_color_t color);
int ui_renderer_draw_text(uint32_t x, uint32_t y, const char *text, ui_color_t color);
int ui_renderer_clear_surface(ui_render_context_t *context);
int ui_renderer_present(void);
int ui_renderer_set_clip_region(uint32_t x, uint32_t y, uint32_t width, uint32_t height);
int ui_renderer_get_stats(ui_renderer_stats_t *stats);

#endif
