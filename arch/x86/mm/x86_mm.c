#include "x86_mm.h"

int x86_mm_init(void)
{
    return 0;
}

int x86_mm_setup_page_tables(uint32_t kernel_base, uint32_t kernel_size)
{
    if (kernel_base == 0 || kernel_size == 0)
    {
        return -1;
    }
    return 0;
}

int x86_mm_allocate_page(uint32_t *phys_addr)
{
    if (!phys_addr)
    {
        return -1;
    }
    return 0;
}

int x86_mm_free_page(uint32_t phys_addr)
{
    if (phys_addr == 0)
    {
        return -1;
    }
    return 0;
}

int x86_mm_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags)
{
    if (virt_addr == 0 || phys_addr == 0)
    {
        return -1;
    }
    return 0;
}

int x86_mm_unmap_page(uint32_t virt_addr)
{
    if (virt_addr == 0)
    {
        return -1;
    }
    return 0;
}

int x86_mm_get_stats(x86_memory_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
