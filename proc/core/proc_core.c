#include "proc_core.h"

int proc_core_init(void)
{
    return 0;
}

int proc_core_get_process(uint64_t pid, proc_core_process_t *process)
{
    if (!process)
    {
        return -1;
    }
    return 0;
}

int proc_core_list_processes(proc_core_process_t *processes, uint32_t *count)
{
    if (!processes || !count)
    {
        return -1;
    }
    return 0;
}

int proc_core_get_memory_info(uint64_t pid, uint64_t *memory)
{
    if (!memory)
    {
        return -1;
    }
    return 0;
}

int proc_core_get_cpu_info(uint64_t pid, uint32_t *cpu_usage)
{
    if (!cpu_usage)
    {
        return -1;
    }
    return 0;
}

int proc_core_get_state(proc_core_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
