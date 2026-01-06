#include "ui_system.h"

int ui_init(void)
{
    return 0;
}

int ui_create_window(uint32_t width, uint32_t height, void **window)
{
    if (width == 0 || height == 0 || !window)
    {
        return -1;
    }
    return 0;
}

int ui_destroy_window(void *window)
{
    if (!window)
    {
        return -1;
    }
    return 0;
}

int ui_draw_pixel(void *window, uint32_t x, uint32_t y, uint32_t color)
{
    if (!window)
    {
        return -1;
    }
    return 0;
}

int ui_render_text(void *window, uint32_t x, uint32_t y, const char *text)
{
    if (!window || !text)
    {
        return -1;
    }
    return 0;
}

int ui_get_display_info(ui_display_t *display)
{
    if (!display)
    {
        return -1;
    }
    display->width = 1920;
    display->height = 1080;
    display->bits_per_pixel = 32;
    return 0;
}

int ui_handle_input(void **input_event)
{
    if (!input_event)
    {
        return -1;
    }
    return 0;
}
