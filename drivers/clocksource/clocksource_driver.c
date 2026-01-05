#include "clocksource_driver.h"

static clocksource_t clocksources[32];
static int cs_count = 0;

int clocksource_init(void)
{
    return 0;
}

int clocksource_register(clocksource_t *cs)
{
    if (!cs || cs_count >= 32)
    {
        return -1;
    }
    clocksources[cs_count++] = *cs;
    return 0;
}

int clocksource_get_time(uint32_t id, uint64_t *time)
{
    if (id >= cs_count || !time)
    {
        return -1;
    }
    *time = clocksources[id].counter;
    return 0;
}

int clocksource_set_frequency(uint32_t id, uint64_t freq)
{
    if (id >= cs_count)
    {
        return -1;
    }
    clocksources[id].frequency = freq;
    return 0;
}
