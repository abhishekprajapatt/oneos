#include "ui_event.h"

int ui_event_init(void)
{
    return 0;
}

int ui_event_post(ui_event_t *event)
{
    if (!event || event->source_widget_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_event_poll(ui_event_t *event)
{
    if (!event)
    {
        return -1;
    }
    return 0;
}

int ui_event_dispatch(uint32_t widget_id)
{
    if (widget_id == 0)
    {
        return -1;
    }
    return 0;
}

int ui_event_register_handler(uint32_t widget_id, uint32_t event_type, void *handler)
{
    if (widget_id == 0 || event_type == 0 || !handler)
    {
        return -1;
    }
    return 0;
}

int ui_event_unregister_handler(uint32_t widget_id, uint32_t event_type)
{
    if (widget_id == 0 || event_type == 0)
    {
        return -1;
    }
    return 0;
}

int ui_event_clear_queue(void)
{
    return 0;
}

int ui_event_get_stats(ui_event_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
