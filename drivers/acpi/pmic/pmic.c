#include "pmic.h"

int pmic_init(void)
{
    return 0;
}

int pmic_read_voltage(uint32_t id, uint32_t *voltage)
{
    if (!voltage)
    {
        return -1;
    }
    *voltage = 0;
    return 0;
}

int pmic_read_current(uint32_t id, uint32_t *current)
{
    if (!current)
    {
        return -1;
    }
    *current = 0;
    return 0;
}

int pmic_read_temperature(uint32_t id, uint32_t *temp)
{
    if (!temp)
    {
        return -1;
    }
    *temp = 0;
    return 0;
}

int pmic_set_voltage(uint32_t id, uint32_t voltage)
{
    return 0;
}
