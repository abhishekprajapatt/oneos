#ifndef PROC_CORE_H
#define PROC_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t proc_id;
    uint64_t process_id;
    const char *process_name;
    uint32_t state;
    uint64_t memory_usage;
} proc_core_process_t;

typedef struct
{
    uint32_t total_processes;
    uint32_t running_processes;
    uint32_t sleeping_processes;
    uint64_t total_memory_used;
} proc_core_state_t;

int proc_core_init(void);
int proc_core_get_process(uint64_t pid, proc_core_process_t *process);
int proc_core_list_processes(proc_core_process_t *processes, uint32_t *count);
int proc_core_get_memory_info(uint64_t pid, uint64_t *memory);
int proc_core_get_cpu_info(uint64_t pid, uint32_t *cpu_usage);
int proc_core_get_state(proc_core_state_t *state);

#endif
