#ifndef MM_PAGING_H
#define MM_PAGING_H

#include <stdint.h>

typedef struct
{
    uint32_t page_id;
    uint64_t virtual_addr;
    uint64_t physical_addr;
    uint32_t flags;
    uint32_t access_count;
} mm_paging_entry_t;

typedef struct
{
    uint32_t page_count;
    uint32_t mapped_pages;
    uint32_t unmapped_pages;
    uint64_t page_faults;
} mm_paging_state_t;

int mm_paging_init(void);
int mm_paging_create_page(uint32_t flags, uint64_t *page_addr);
int mm_paging_map_page(uint64_t virtual_addr, uint64_t physical_addr, uint32_t flags);
int mm_paging_unmap_page(uint64_t virtual_addr);
int mm_paging_get_physical_addr(uint64_t virtual_addr, uint64_t *physical_addr);
int mm_paging_page_fault_handler(uint64_t fault_addr);

#endif
