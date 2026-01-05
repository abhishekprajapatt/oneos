#include "x86_kernel.h"

int x86_kernel_init(void)
{
    return 0;
}

int x86_kernel_setup_smp(void)
{
    return 0;
}

int x86_kernel_online_cpu(uint32_t cpu_id)
{
    if (cpu_id == 0)
    {
        return -1;
    }
    return 0;
}

int x86_kernel_offline_cpu(uint32_t cpu_id)
{
    if (cpu_id == 0)
    {
        return -1;
    }
    return 0;
}

int x86_kernel_get_cpu_info(uint32_t cpu_id, x86_cpu_info_t *info)
{
    if (cpu_id == 0 || !info)
    {
        return -1;
    }
    return 0;
}

int x86_kernel_send_ipi(uint32_t target_cpu, uint32_t interrupt_vector)
{
    if (target_cpu == 0)
    {
        return -1;
    }
    return 0;
}

int x86_kernel_get_stats(x86_kernel_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
