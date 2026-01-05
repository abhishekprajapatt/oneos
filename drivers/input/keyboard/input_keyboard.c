#include "input_keyboard.h"

int input_keyboard_init(void)
{
    return 0;
}

int input_keyboard_probe(input_keyboard_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_keyboard_read_event(input_keyboard_device_t *dev, input_keyboard_event_t *event)
{
    if (!dev || !event)
    {
        return -1;
    }
    return 0;
}

int input_keyboard_set_leds(input_keyboard_device_t *dev, uint8_t leds)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_keyboard_get_layout(input_keyboard_device_t *dev, uint8_t *layout)
{
    if (!dev || !layout)
    {
        return -1;
    }
    return 0;
}

int input_keyboard_set_repeat_rate(input_keyboard_device_t *dev, uint8_t delay, uint8_t rate)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
