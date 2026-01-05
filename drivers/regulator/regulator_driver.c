#include "regulator_driver.h"

int regulator_init(void)
{
    return 0;
}

int regulator_register(regulator_t *reg)
{
    if (!reg)
    {
        return -1;
    }
    return 0;
}

int regulator_enable(uint32_t id)
{
    return 0;
}

int regulator_disable(uint32_t id)
{
    return 0;
}

int regulator_set_voltage(uint32_t id, uint32_t voltage)
{
    return 0;
}

int regulator_get_voltage(uint32_t id, uint32_t *voltage)
{
    if (!voltage)
    {
        return -1;
    }
    *voltage = 0;
    return 0;
}
