#include "x86_boot.h"

int x86_boot_init(void)
{
    return 0;
}

int x86_boot_setup_gdt(void)
{
    return 0;
}

int x86_boot_setup_idt(void)
{
    return 0;
}

int x86_boot_enable_paging(void)
{
    return 0;
}

int x86_boot_setup_pae(void)
{
    return 0;
}

int x86_boot_jump_to_kernel(uint32_t kernel_entry)
{
    if (kernel_entry == 0)
    {
        return -1;
    }
    return 0;
}

int x86_boot_get_stats(x86_boot_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
