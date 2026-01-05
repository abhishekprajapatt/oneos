#include "kernel_time.h"

int kernel_time_init(void)
{
    return 0;
}

int kernel_time_get_current(kernel_time_value_t *time)
{
    if (!time)
    {
        return -1;
    }
    return 0;
}

int kernel_time_set_current(kernel_time_value_t *time)
{
    if (!time)
    {
        return -1;
    }
    return 0;
}

int kernel_time_create_timer(kernel_time_timer_t *timer)
{
    if (!timer)
    {
        return -1;
    }
    return 0;
}

int kernel_time_delete_timer(uint32_t timer_id)
{
    return 0;
}

int kernel_time_get_resolution(uint32_t *resolution_ns)
{
    if (!resolution_ns)
    {
        return -1;
    }
    return 0;
}
