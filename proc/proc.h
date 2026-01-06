#ifndef PROC_H
#define PROC_H

#include <stdint.h>

typedef struct
{
    uint32_t pid;
    uint32_t ppid;
    uint32_t memory;
    uint32_t cpu_time;
} proc_info_t;

int proc_init(void);
int proc_read_stat(uint32_t pid, proc_info_t *info);
int proc_list_processes(uint32_t **pids, uint32_t *count);
int proc_get_memory_info(uint32_t pid, uint32_t *memory);
int proc_get_cpu_time(uint32_t pid, uint32_t *cpu_time);

#endif
