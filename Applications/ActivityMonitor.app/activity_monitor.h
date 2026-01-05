#ifndef ACTIVITY_MONITOR_H
#define ACTIVITY_MONITOR_H

#include <stdint.h>

typedef struct
{
    uint32_t process_id;
    const char *process_name;
    uint32_t cpu_usage;
    uint64_t memory_usage;
    uint32_t thread_count;
} process_info_t;

typedef struct
{
    uint32_t total_processes;
    uint64_t total_memory;
    uint64_t available_memory;
    uint32_t cpu_count;
} monitor_stats_t;

int activity_monitor_init(void);
int activity_monitor_list_processes(process_info_t *processes, uint32_t *count);
int activity_monitor_get_process_info(uint32_t process_id, process_info_t *info);
int activity_monitor_kill_process(uint32_t process_id);
int activity_monitor_get_cpu_usage(uint32_t *usage);
int activity_monitor_get_memory_usage(uint64_t *usage);
int activity_monitor_get_stats(monitor_stats_t *stats);

#endif
