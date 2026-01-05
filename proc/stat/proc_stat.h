#ifndef PROC_STAT_H
#define PROC_STAT_H

#include <stdint.h>

typedef struct
{
    uint64_t user_time;
    uint64_t system_time;
    uint64_t idle_time;
    uint64_t iowait_time;
    uint32_t cpu_count;
} proc_stat_cpu_t;

typedef struct
{
    uint32_t total_cpu_samples;
    uint64_t total_cpu_time;
    uint32_t load_average_1min;
    uint32_t load_average_5min;
    uint32_t load_average_15min;
} proc_stat_state_t;

int proc_stat_init(void);
int proc_stat_read_cpu_info(proc_stat_cpu_t *cpu);
int proc_stat_read_load_average(uint32_t *load_1, uint32_t *load_5, uint32_t *load_15);
int proc_stat_read_system_uptime(uint64_t *uptime);
int proc_stat_read_context_switches(uint64_t *switches);
int proc_stat_get_state(proc_stat_state_t *state);

#endif
