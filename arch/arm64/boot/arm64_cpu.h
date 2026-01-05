#ifndef ARCH_ARM64_CPU_H
#define ARCH_ARM64_CPU_H

#include <stdint.h>

typedef struct
{
    uint32_t cpu_id;
    uint32_t online;
    uint64_t frequency;
    uint32_t temperature;
} arm64_cpu_info_t;

typedef struct
{
    uint32_t total_cpus;
    uint32_t online_cpus;
    uint32_t offline_cpus;
    uint64_t aggregate_frequency;
} arm64_cpu_stats_t;

int arm64_cpu_init(void);
int arm64_cpu_online(uint32_t cpu_id);
int arm64_cpu_offline(uint32_t cpu_id);
int arm64_cpu_get_info(uint32_t cpu_id, arm64_cpu_info_t *info);
int arm64_cpu_set_frequency(uint32_t cpu_id, uint64_t frequency);
int arm64_cpu_get_temperature(uint32_t cpu_id, uint32_t *temperature);
int arm64_cpu_get_stats(arm64_cpu_stats_t *stats);

#endif
