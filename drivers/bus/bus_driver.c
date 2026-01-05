#include "bus_driver.h"

int bus_init(void)
{
    return 0;
}

int bus_register_type(bus_type_t *bus)
{
    if (!bus)
    {
        return -1;
    }
    return 0;
}

int bus_add_device(bus_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int bus_remove_device(uint32_t id)
{
    return 0;
}

int bus_scan(uint32_t bus_id)
{
    return 0;
}
