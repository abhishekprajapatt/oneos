#include "proc.h"

int proc_init(void)
{
    return 0;
}

int proc_read_stat(uint32_t pid, proc_info_t *info)
{
    if (pid == 0 || !info)
    {
        return -1;
    }
    info->pid = pid;
    info->ppid = 1;
    info->memory = 0;
    info->cpu_time = 0;
    return 0;
}

int proc_list_processes(uint32_t **pids, uint32_t *count)
{
    if (!pids || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int proc_get_memory_info(uint32_t pid, uint32_t *memory)
{
    if (pid == 0 || !memory)
    {
        return -1;
    }
    *memory = 0;
    return 0;
}

int proc_get_cpu_time(uint32_t pid, uint32_t *cpu_time)
{
    if (pid == 0 || !cpu_time)
    {
        return -1;
    }
    *cpu_time = 0;
    return 0;
}
