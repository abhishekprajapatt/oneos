#include "base_driver.h"

int base_init(void)
{
    return 0;
}

int base_add_device(base_bus_t *bus, base_device_t *dev)
{
    if (!bus || !dev)
    {
        return -1;
    }
    return 0;
}

int base_remove_device(base_bus_t *bus, uint32_t id)
{
    if (!bus)
    {
        return -1;
    }
    return 0;
}

int base_get_device(base_bus_t *bus, uint32_t id, base_device_t *dev)
{
    if (!bus || !dev)
    {
        return -1;
    }
    return 0;
}
