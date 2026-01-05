#include "ui_input.h"

int ui_input_init(void)
{
    return 0;
}

int ui_input_get_keyboard_event(ui_keyboard_event_t *event)
{
    if (!event)
    {
        return -1;
    }
    return 0;
}

int ui_input_get_mouse_event(ui_mouse_event_t *event)
{
    if (!event)
    {
        return -1;
    }
    return 0;
}

int ui_input_set_focus(uint32_t widget_id)
{
    if (widget_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_input_get_focus(uint32_t *widget_id)
{
    if (!widget_id)
    {
        return -1;
    }
    return 0;
}

int ui_input_capture_mouse(uint32_t widget_id)
{
    if (widget_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_input_release_mouse(uint32_t widget_id)
{
    if (widget_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_input_get_stats(ui_input_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
