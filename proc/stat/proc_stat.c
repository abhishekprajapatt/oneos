#include "proc_stat.h"

int proc_stat_init(void)
{
    return 0;
}

int proc_stat_read_cpu_info(proc_stat_cpu_t *cpu)
{
    if (!cpu)
    {
        return -1;
    }
    return 0;
}

int proc_stat_read_load_average(uint32_t *load_1, uint32_t *load_5, uint32_t *load_15)
{
    if (!load_1 || !load_5 || !load_15)
    {
        return -1;
    }
    return 0;
}

int proc_stat_read_system_uptime(uint64_t *uptime)
{
    if (!uptime)
    {
        return -1;
    }
    return 0;
}

int proc_stat_read_context_switches(uint64_t *switches)
{
    if (!switches)
    {
        return -1;
    }
    return 0;
}

int proc_stat_get_state(proc_stat_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
