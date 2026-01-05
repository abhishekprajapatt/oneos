#include "proc_cpuinfo.h"

int proc_cpuinfo_init(void)
{
    return 0;
}

int proc_cpuinfo_get_cpu(uint32_t cpu_id, proc_cpuinfo_cpu_t *cpu)
{
    if (!cpu)
    {
        return -1;
    }
    return 0;
}

int proc_cpuinfo_list_cpus(proc_cpuinfo_cpu_t *cpus, uint32_t *count)
{
    if (!cpus || !count)
    {
        return -1;
    }
    return 0;
}

int proc_cpuinfo_get_cache_info(uint32_t cpu_id, uint64_t *cache_size)
{
    if (!cache_size)
    {
        return -1;
    }
    return 0;
}

int proc_cpuinfo_get_flags(uint32_t cpu_id, char *flags, uint32_t *size)
{
    if (!flags || !size)
    {
        return -1;
    }
    return 0;
}

int proc_cpuinfo_get_state(proc_cpuinfo_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
