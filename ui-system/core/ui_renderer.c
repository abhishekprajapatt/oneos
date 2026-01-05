#include "ui_renderer.h"

int ui_renderer_init(void)
{
    return 0;
}

int ui_renderer_draw_rect(ui_render_context_t *context)
{
    if (!context || context->widget_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_renderer_draw_circle(uint32_t x, uint32_t y, uint32_t radius, ui_color_t color)
{
    if (radius == 0)
    {
        return -1;
    }
    return 0;
}

int ui_renderer_draw_text(uint32_t x, uint32_t y, const char *text, ui_color_t color)
{
    if (!text)
    {
        return -1;
    }
    return 0;
}

int ui_renderer_clear_surface(ui_render_context_t *context)
{
    if (!context)
    {
        return -1;
    }
    return 0;
}

int ui_renderer_present(void)
{
    return 0;
}

int ui_renderer_set_clip_region(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (width == 0 || height == 0)
    {
        return -1;
    }
    return 0;
}

int ui_renderer_get_stats(ui_renderer_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
