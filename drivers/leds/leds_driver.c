#include "leds_driver.h"

int leds_init(void)
{
    return 0;
}

int leds_register(led_t *led)
{
    if (!led)
    {
        return -1;
    }
    return 0;
}

int leds_unregister(uint32_t id)
{
    return 0;
}

int leds_set_brightness(uint32_t id, uint32_t brightness)
{
    return 0;
}

int leds_get_brightness(uint32_t id, uint32_t *brightness)
{
    if (!brightness)
    {
        return -1;
    }
    *brightness = 0;
    return 0;
}
