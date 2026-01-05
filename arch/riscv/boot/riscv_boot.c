#include "riscv_boot.h"

int riscv_boot_init(void)
{
    return 0;
}

int riscv_boot_setup_paging(void)
{
    return 0;
}

int riscv_boot_enable_supervisor(void)
{
    return 0;
}

int riscv_boot_setup_traps(void)
{
    return 0;
}

int riscv_boot_configure_timer(void)
{
    return 0;
}

int riscv_boot_jump_to_kernel(uint64_t kernel_entry)
{
    if (kernel_entry == 0)
    {
        return -1;
    }
    return 0;
}

int riscv_boot_get_stats(riscv_boot_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
