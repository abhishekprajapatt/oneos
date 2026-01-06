#include "coregraphics.h"

int coregraphics_init(void)
{
    return 0;
}

int coregraphics_create_context(uint32_t width, uint32_t height, CGContext **ctx)
{
    if (width == 0 || height == 0 || !ctx)
    {
        return -1;
    }
    return 0;
}

int coregraphics_release_context(CGContext *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int coregraphics_draw_line(CGContext *ctx, float x1, float y1, float x2, float y2)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int coregraphics_fill_rect(CGContext *ctx, float x, float y, float width, float height)
{
    if (!ctx || width == 0 || height == 0)
    {
        return -1;
    }
    return 0;
}

int coregraphics_set_color(CGContext *ctx, uint32_t color)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}
