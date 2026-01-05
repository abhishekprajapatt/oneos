#include "ui_core.h"

int ui_core_init(void)
{
    return 0;
}

int ui_core_create_window(const char *title, uint32_t width, uint32_t height, ui_window_t *window)
{
    if (!title || width == 0 || height == 0 || !window)
    {
        return -1;
    }
    return 0;
}

int ui_core_destroy_window(uint32_t window_id)
{
    if (window_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_core_show_window(uint32_t window_id)
{
    if (window_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_core_hide_window(uint32_t window_id)
{
    if (window_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_core_set_window_properties(uint32_t window_id, uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    if (window_id == 0 || width == 0 || height == 0)
    {
        return -1;
    }
    return 0;
}

int ui_core_list_windows(ui_window_t *windows, uint32_t *count)
{
    if (!windows || !count)
    {
        return -1;
    }
    return 0;
}

int ui_core_get_stats(ui_core_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}

int ui_core_cleanup(void)
{
    return 0;
}
