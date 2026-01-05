#include "activity_monitor.h"

int activity_monitor_init(void)
{
    return 0;
}

int activity_monitor_list_processes(process_info_t *processes, uint32_t *count)
{
    if (!processes || !count)
    {
        return -1;
    }
    return 0;
}

int activity_monitor_get_process_info(uint32_t process_id, process_info_t *info)
{
    if (process_id == 0 || !info)
    {
        return -1;
    }
    return 0;
}

int activity_monitor_kill_process(uint32_t process_id)
{
    if (process_id == 0)
    {
        return -1;
    }
    return 0;
}

int activity_monitor_get_cpu_usage(uint32_t *usage)
{
    if (!usage)
    {
        return -1;
    }
    return 0;
}

int activity_monitor_get_memory_usage(uint64_t *usage)
{
    if (!usage)
    {
        return -1;
    }
    return 0;
}

int activity_monitor_get_stats(monitor_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
