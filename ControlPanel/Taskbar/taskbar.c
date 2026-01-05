#include "taskbar.h"

int taskbar_init(void)
{
    return 0;
}

int taskbar_list_items(taskbar_item_t *items, uint32_t *count)
{
    if (!items || !count)
    {
        return -1;
    }
    return 0;
}

int taskbar_add_item(uint32_t window_id, taskbar_item_t *item)
{
    if (window_id == 0 || !item)
    {
        return -1;
    }
    return 0;
}

int taskbar_remove_item(uint32_t item_id)
{
    if (item_id == 0)
    {
        return -1;
    }
    return 0;
}

int taskbar_activate_item(uint32_t item_id)
{
    if (item_id == 0)
    {
        return -1;
    }
    return 0;
}

int taskbar_pin_item(uint32_t item_id)
{
    if (item_id == 0)
    {
        return -1;
    }
    return 0;
}

int taskbar_unpin_item(uint32_t item_id)
{
    if (item_id == 0)
    {
        return -1;
    }
    return 0;
}

int taskbar_set_position(uint32_t position)
{
    return 0;
}

int taskbar_toggle_auto_hide(uint32_t enable)
{
    return 0;
}

int taskbar_get_stats(taskbar_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
