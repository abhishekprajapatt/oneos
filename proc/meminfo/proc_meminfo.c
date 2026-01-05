#include "proc_meminfo.h"

int proc_meminfo_init(void)
{
    return 0;
}

int proc_meminfo_read_memory(proc_meminfo_memory_t *memory)
{
    if (!memory)
    {
        return -1;
    }
    return 0;
}

int proc_meminfo_read_swap(proc_meminfo_swap_t *swap)
{
    if (!swap)
    {
        return -1;
    }
    return 0;
}

int proc_meminfo_get_memory_pressure(uint32_t *pressure)
{
    if (!pressure)
    {
        return -1;
    }
    return 0;
}

int proc_meminfo_get_fragmentation(uint32_t *fragmentation)
{
    if (!fragmentation)
    {
        return -1;
    }
    return 0;
}

int proc_meminfo_get_state(proc_meminfo_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
