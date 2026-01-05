#ifndef PROC_MEMINFO_H
#define PROC_MEMINFO_H

#include <stdint.h>

typedef struct
{
    uint64_t mem_total;
    uint64_t mem_free;
    uint64_t mem_available;
    uint64_t mem_used;
    uint64_t buffers;
    uint64_t cached;
} proc_meminfo_memory_t;

typedef struct
{
    uint64_t swap_total;
    uint64_t swap_free;
    uint64_t swap_used;
} proc_meminfo_swap_t;

typedef struct
{
    uint32_t total_samples;
    uint64_t peak_memory_usage;
    uint64_t average_memory_usage;
} proc_meminfo_state_t;

int proc_meminfo_init(void);
int proc_meminfo_read_memory(proc_meminfo_memory_t *memory);
int proc_meminfo_read_swap(proc_meminfo_swap_t *swap);
int proc_meminfo_get_memory_pressure(uint32_t *pressure);
int proc_meminfo_get_fragmentation(uint32_t *fragmentation);
int proc_meminfo_get_state(proc_meminfo_state_t *state);

#endif
