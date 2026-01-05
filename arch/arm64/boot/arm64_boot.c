#include "arm64_boot.h"

int arm64_boot_init(void)
{
    return 0;
}

int arm64_boot_setup_mmu(void)
{
    return 0;
}

int arm64_boot_enable_caches(void)
{
    return 0;
}

int arm64_boot_initialize_gic(void)
{
    return 0;
}

int arm64_boot_setup_stack(uint64_t stack_base, uint64_t stack_size)
{
    if (stack_base == 0 || stack_size == 0)
    {
        return -1;
    }
    return 0;
}

int arm64_boot_jump_to_kernel(uint64_t kernel_entry)
{
    if (kernel_entry == 0)
    {
        return -1;
    }
    return 0;
}

int arm64_boot_get_stats(arm64_boot_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
