#include "regulator_buck.h"

int regulator_buck_init(void)
{
    return 0;
}

int regulator_buck_probe(regulator_buck_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int regulator_buck_set_voltage(regulator_buck_device_t *dev, uint32_t voltage)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int regulator_buck_get_voltage(regulator_buck_device_t *dev, uint32_t *voltage)
{
    if (!dev || !voltage)
    {
        return -1;
    }
    return 0;
}

int regulator_buck_enable(regulator_buck_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int regulator_buck_disable(regulator_buck_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
