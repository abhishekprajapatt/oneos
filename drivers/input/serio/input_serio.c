#include "input_serio.h"

int input_serio_init(void)
{
    return 0;
}

int input_serio_probe(input_serio_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_serio_write(input_serio_device_t *dev, uint8_t data)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_serio_read(input_serio_device_t *dev, uint8_t *data)
{
    if (!dev || !data)
    {
        return -1;
    }
    return 0;
}

int input_serio_flush(input_serio_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int input_serio_set_speed(input_serio_device_t *dev, uint32_t speed)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
