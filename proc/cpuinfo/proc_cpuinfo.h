#ifndef PROC_CPUINFO_H
#define PROC_CPUINFO_H

#include <stdint.h>

typedef struct
{
    uint32_t cpu_id;
    uint32_t processor;
    uint32_t vendor_id;
    uint32_t cpu_family;
    uint32_t model;
    uint32_t stepping;
    uint32_t mhz;
} proc_cpuinfo_cpu_t;

typedef struct
{
    uint32_t total_cpus;
    uint32_t online_cpus;
    uint32_t offline_cpus;
    uint32_t cache_levels;
} proc_cpuinfo_state_t;

int proc_cpuinfo_init(void);
int proc_cpuinfo_get_cpu(uint32_t cpu_id, proc_cpuinfo_cpu_t *cpu);
int proc_cpuinfo_list_cpus(proc_cpuinfo_cpu_t *cpus, uint32_t *count);
int proc_cpuinfo_get_cache_info(uint32_t cpu_id, uint64_t *cache_size);
int proc_cpuinfo_get_flags(uint32_t cpu_id, char *flags, uint32_t *size);
int proc_cpuinfo_get_state(proc_cpuinfo_state_t *state);

#endif
