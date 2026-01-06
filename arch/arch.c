#include "arch.h"

static arch_info_t arch_current_info = {0, 0, 0};

int arch_init(void)
{
    arch_current_info.version = 1;
    arch_current_info.cpu_count = 4;
    arch_current_info.page_size = 4096;
    return 0;
}

int arch_get_info(arch_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    info->version = arch_current_info.version;
    info->cpu_count = arch_current_info.cpu_count;
    info->page_size = arch_current_info.page_size;
    return 0;
}

int arch_setup_memory(uint32_t base, uint32_t size)
{
    if (size == 0)
    {
        return -1;
    }
    return 0;
}

int arch_setup_interrupts(void)
{
    return 0;
}

int arch_start_cpu(uint32_t cpu_id)
{
    if (cpu_id >= 4)
    {
        return -1;
    }
    return 0;
}
