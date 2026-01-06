#include "timer_driver.h"

int timer_driver_init(void)
{
    return 0;
}

int timer_driver_set_timeout(uint32_t milliseconds)
{
    if (milliseconds == 0)
    {
        return -1;
    }
    return 0;
}

int timer_driver_cancel_timeout(void)
{
    return 0;
}

int timer_driver_get_ticks(uint64_t *ticks)
{
    if (!ticks)
    {
        return -1;
    }
    *ticks = 0;
    return 0;
}

int timer_driver_sleep(uint32_t milliseconds)
{
    if (milliseconds == 0)
    {
        return -1;
    }
    return 0;
}
