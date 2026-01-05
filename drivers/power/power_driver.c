#include "power_driver.h"

int power_init(void)
{
    return 0;
}

int power_register_device(power_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int power_set_state(power_device_t *dev, uint32_t state)
{
    if (!dev)
    {
        return -1;
    }
    dev->power_state = state;
    return 0;
}

int power_get_state(power_device_t *dev, uint32_t *state)
{
    if (!dev || !state)
    {
        return -1;
    }
    *state = dev->power_state;
    return 0;
}

int power_wake_lock(power_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int power_wake_unlock(power_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
