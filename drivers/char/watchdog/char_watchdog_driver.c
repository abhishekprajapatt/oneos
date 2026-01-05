#include "char_watchdog_driver.h"

int char_watchdog_init(void)
{
    return 0;
}

int char_watchdog_register(char_watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int char_watchdog_unregister(uint32_t device_id)
{
    return 0;
}

int char_watchdog_start(char_watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->enabled = 1;
    return 0;
}

int char_watchdog_stop(char_watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    dev->enabled = 0;
    return 0;
}

int char_watchdog_ping(char_watchdog_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int char_watchdog_set_timeout(char_watchdog_device_t *dev, uint32_t timeout)
{
    if (!dev)
    {
        return -1;
    }
    dev->timeout = timeout;
    return 0;
}
