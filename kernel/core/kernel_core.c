#include "kernel_core.h"

int kernel_core_init(void)
{
    return 0;
}

int kernel_core_get_info(uint32_t core_id, kernel_core_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int kernel_core_set_freq(uint32_t core_id, uint32_t freq)
{
    return 0;
}

int kernel_core_enable(uint32_t core_id)
{
    return 0;
}

int kernel_core_disable(uint32_t core_id)
{
    return 0;
}

int kernel_core_get_load(uint32_t core_id, uint32_t *load)
{
    if (!load)
    {
        return -1;
    }
    return 0;
}
