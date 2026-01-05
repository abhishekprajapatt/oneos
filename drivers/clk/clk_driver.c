#include "clk_driver.h"

static clk_t clocks[256];
static int clock_count = 0;

int clk_init(void)
{
    return 0;
}

int clk_register(clk_t *clk)
{
    if (!clk || clock_count >= 256)
    {
        return -1;
    }
    clocks[clock_count++] = *clk;
    return 0;
}

int clk_unregister(uint32_t id)
{
    if (id >= clock_count)
    {
        return -1;
    }
    return 0;
}

int clk_enable(uint32_t id)
{
    if (id >= clock_count)
    {
        return -1;
    }
    return 0;
}

int clk_disable(uint32_t id)
{
    if (id >= clock_count)
    {
        return -1;
    }
    return 0;
}

int clk_set_rate(uint32_t id, uint32_t rate)
{
    if (id >= clock_count)
    {
        return -1;
    }
    clocks[id].rate = rate;
    return 0;
}

uint32_t clk_get_rate(uint32_t id)
{
    if (id >= clock_count)
    {
        return 0;
    }
    return clocks[id].rate;
}
