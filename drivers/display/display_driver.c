#include "display_driver.h"

int display_init(void)
{
    return 0;
}

int display_open(display_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_close(display_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int display_set_mode(display_device_t *dev, display_mode_t *mode)
{
    if (!dev || !mode)
    {
        return -1;
    }
    dev->mode = *mode;
    return 0;
}

int display_get_framebuffer(display_device_t *dev, void **buffer)
{
    if (!dev || !buffer)
    {
        return -1;
    }
    *buffer = dev->framebuffer;
    return 0;
}

int display_update(display_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
