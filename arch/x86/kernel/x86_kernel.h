#ifndef ARCH_X86_KERNEL_H
#define ARCH_X86_KERNEL_H

#include <stdint.h>

typedef struct
{
    uint32_t cpu_id;
    uint32_t online;
    uint32_t frequency;
    uint32_t cache_size;
} x86_cpu_info_t;

typedef struct
{
    uint32_t total_cpus;
    uint32_t online_cpus;
    uint32_t smp_enabled;
    uint64_t aggregate_frequency;
} x86_kernel_stats_t;

int x86_kernel_init(void);
int x86_kernel_setup_smp(void);
int x86_kernel_online_cpu(uint32_t cpu_id);
int x86_kernel_offline_cpu(uint32_t cpu_id);
int x86_kernel_get_cpu_info(uint32_t cpu_id, x86_cpu_info_t *info);
int x86_kernel_send_ipi(uint32_t target_cpu, uint32_t interrupt_vector);
int x86_kernel_get_stats(x86_kernel_stats_t *stats);

#endif
