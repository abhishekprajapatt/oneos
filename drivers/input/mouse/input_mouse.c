#include "input_mouse.h"

int input_mouse_init(void)
{
    return 0;
}

int input_mouse_probe(input_mouse_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_mouse_read_event(input_mouse_device_t *dev, input_mouse_event_t *event)
{
    if (!dev || !event)
    {
        return -1;
    }
    return 0;
}

int input_mouse_set_resolution(input_mouse_device_t *dev, uint8_t resolution)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_mouse_set_sample_rate(input_mouse_device_t *dev, uint8_t rate)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_mouse_get_id(input_mouse_device_t *dev, uint8_t *id)
{
    if (!dev || !id)
    {
        return -1;
    }
    return 0;
}
