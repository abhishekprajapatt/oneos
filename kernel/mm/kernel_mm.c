#include "kernel_mm.h"

int kernel_mm_init(void)
{
    return 0;
}

int kernel_mm_allocate_page(uint32_t flags, kernel_mm_page_t *page)
{
    if (!page)
    {
        return -1;
    }
    return 0;
}

int kernel_mm_free_page(uint32_t page_id)
{
    return 0;
}

int kernel_mm_map_page(uint64_t virtual_addr, uint64_t physical_addr)
{
    return 0;
}

int kernel_mm_unmap_page(uint64_t virtual_addr)
{
    return 0;
}

int kernel_mm_get_memory_info(kernel_mm_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
