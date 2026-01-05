#include "input_joystick.h"

int input_joystick_init(void)
{
    return 0;
}

int input_joystick_probe(input_joystick_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_joystick_read_state(input_joystick_device_t *dev, input_joystick_event_t *event)
{
    if (!dev || !event)
    {
        return -1;
    }
    return 0;
}

int input_joystick_set_rumble(input_joystick_device_t *dev, uint8_t strength, uint16_t duration)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_joystick_calibrate(input_joystick_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_joystick_get_capabilities(input_joystick_device_t *dev, uint32_t *caps)
{
    if (!dev || !caps)
    {
        return -1;
    }
    return 0;
}
