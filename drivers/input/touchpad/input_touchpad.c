#include "input_touchpad.h"

int input_touchpad_init(void)
{
    return 0;
}

int input_touchpad_probe(input_touchpad_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_touchpad_read_event(input_touchpad_device_t *dev, input_touchpad_event_t *event)
{
    if (!dev || !event)
    {
        return -1;
    }
    return 0;
}

int input_touchpad_set_sensitivity(input_touchpad_device_t *dev, uint8_t sensitivity)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_touchpad_calibrate(input_touchpad_device_t *dev, uint32_t *min_x, uint32_t *max_x, uint32_t *min_y, uint32_t *max_y)
{
    if (!dev || !min_x || !max_x || !min_y || !max_y)
    {
        return -1;
    }
    return 0;
}

int input_touchpad_get_resolution(input_touchpad_device_t *dev, uint32_t *x_res, uint32_t *y_res)
{
    if (!dev || !x_res || !y_res)
    {
        return -1;
    }
    return 0;
}
