#include "power_battery.h"

int power_battery_init(void)
{
    return 0;
}

int power_battery_probe(power_battery_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int power_battery_get_info(power_battery_device_t *dev, power_battery_info_t *info)
{
    if (!dev || !info)
    {
        return -1;
    }
    return 0;
}

int power_battery_set_charge_current(power_battery_device_t *dev, uint32_t current)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int power_battery_enable_charging(power_battery_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int power_battery_disable_charging(power_battery_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
