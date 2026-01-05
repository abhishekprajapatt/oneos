#include "display.h"

int display_init(void)
{
    return 0;
}

int display_list_connected(display_settings_t *displays, uint32_t *count)
{
    if (!displays || !count)
    {
        return -1;
    }
    return 0;
}

int display_get_current(display_settings_t *settings)
{
    if (!settings)
    {
        return -1;
    }
    return 0;
}

int display_set_resolution(uint32_t display_id, uint32_t width, uint32_t height)
{
    if (display_id == 0 || width == 0 || height == 0)
    {
        return -1;
    }
    return 0;
}

int display_set_refresh_rate(uint32_t display_id, uint32_t refresh_rate)
{
    if (display_id == 0 || refresh_rate == 0)
    {
        return -1;
    }
    return 0;
}

int display_set_brightness(uint32_t display_id, uint32_t brightness)
{
    if (display_id == 0 || brightness > 100)
    {
        return -1;
    }
    return 0;
}

int display_set_contrast(uint32_t display_id, uint32_t contrast)
{
    if (display_id == 0 || contrast > 100)
    {
        return -1;
    }
    return 0;
}

int display_get_stats(display_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
