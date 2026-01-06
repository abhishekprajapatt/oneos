#include "driver_base.h"

int driver_base_init(void)
{
    return 0;
}

int driver_register(driver_t *driver)
{
    if (!driver || !driver->name)
    {
        return -1;
    }
    return 0;
}

int driver_unregister(const char *name)
{
    if (!name)
    {
        return -1;
    }
    return 0;
}

int driver_find(const char *name, driver_t **driver)
{
    if (!name || !driver)
    {
        return -1;
    }
    return 0;
}

int driver_probe_all(void)
{
    return 0;
}
