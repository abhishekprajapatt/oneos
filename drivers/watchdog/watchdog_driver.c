#include "watchdog_driver.h"

int watchdog_init(void)
{
    return 0;
}

int watchdog_register(watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int watchdog_start(watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->enabled = 1;
    return 0;
}

int watchdog_stop(watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->enabled = 0;
    return 0;
}

int watchdog_ping(watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int watchdog_set_timeout(watchdog_device_t *dev, uint32_t timeout)
{
    if (!dev)
    {
        return -1;
    }
    dev->timeout = timeout;
    return 0;
}
