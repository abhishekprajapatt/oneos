#include "arm64_cpu.h"

int arm64_cpu_init(void)
{
    return 0;
}

int arm64_cpu_online(uint32_t cpu_id)
{
    if (cpu_id == 0)
    {
        return -1;
    }
    return 0;
}

int arm64_cpu_offline(uint32_t cpu_id)
{
    if (cpu_id == 0)
    {
        return -1;
    }
    return 0;
}

int arm64_cpu_get_info(uint32_t cpu_id, arm64_cpu_info_t *info)
{
    if (cpu_id == 0 || !info)
    {
        return -1;
    }
    return 0;
}

int arm64_cpu_set_frequency(uint32_t cpu_id, uint64_t frequency)
{
    if (cpu_id == 0 || frequency == 0)
    {
        return -1;
    }
    return 0;
}

int arm64_cpu_get_temperature(uint32_t cpu_id, uint32_t *temperature)
{
    if (cpu_id == 0 || !temperature)
    {
        return -1;
    }
    return 0;
}

int arm64_cpu_get_stats(arm64_cpu_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
