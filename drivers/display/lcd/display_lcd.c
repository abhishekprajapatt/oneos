#include "display_lcd.h"

int display_lcd_init(void)
{
    return 0;
}

int display_lcd_probe(display_lcd_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_lcd_set_timing(display_lcd_device_t *dev, display_lcd_timing_t *timing)
{
    if (!dev || !timing)
    {
        return -1;
    }
    return 0;
}

int display_lcd_enable(display_lcd_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_lcd_disable(display_lcd_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_lcd_set_backlight(display_lcd_device_t *dev, uint8_t level)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
