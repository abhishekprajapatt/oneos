#include "mm_paging.h"

int mm_paging_init(void)
{
    return 0;
}

int mm_paging_create_page(uint32_t flags, uint64_t *page_addr)
{
    if (!page_addr)
    {
        return -1;
    }
    return 0;
}

int mm_paging_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint32_t flags)
{
    return 0;
}

int mm_paging_unmap_page(uint64_t virtual_addr)
{
    return 0;
}

int mm_paging_get_physical_addr(uint64_t virtual_addr, uint64_t *physical_addr)
{
    if (!physical_addr)
    {
        return -1;
    }
    return 0;
}

int mm_paging_page_fault_handler(uint64_t fault_addr)
{
    return 0;
}
