#include "leds_rgb.h"

int leds_rgb_init(void)
{
    return 0;
}

int leds_rgb_probe(leds_rgb_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int leds_rgb_set_color(leds_rgb_device_t *dev, leds_rgb_color_t *color)
{
    if (!dev || !color)
    {
        return -1;
    }
    return 0;
}

int leds_rgb_set_brightness(leds_rgb_device_t *dev, uint32_t brightness)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int leds_rgb_enable_pwm(leds_rgb_device_t *dev, uint32_t frequency)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int leds_rgb_disable(leds_rgb_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
