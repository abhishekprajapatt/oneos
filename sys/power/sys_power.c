#include "sys_power.h"

int sys_power_init(void)
{
    return 0;
}

int sys_power_set_state(uint32_t power_state)
{
    return 0;
}

int sys_power_get_battery_level(uint32_t *level)
{
    if (!level)
    {
        return -1;
    }
    return 0;
}

int sys_power_set_power_profile(uint32_t profile)
{
    return 0;
}

int sys_power_set_cpu_freq(uint32_t freq)
{
    return 0;
}

int sys_power_sleep(uint32_t seconds)
{
    return 0;
}

int sys_power_get_info(sys_power_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}
