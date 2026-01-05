#ifndef KERNEL_MM_H
#define KERNEL_MM_H

#include <stdint.h>

typedef struct
{
    uint32_t page_id;
    uint64_t physical_addr;
    uint64_t virtual_addr;
    uint32_t flags;
} kernel_mm_page_t;

typedef struct
{
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t free_memory;
    uint32_t page_count;
} kernel_mm_state_t;

int kernel_mm_init(void);
int kernel_mm_allocate_page(uint32_t flags, kernel_mm_page_t *page);
int kernel_mm_free_page(uint32_t page_id);
int kernel_mm_map_page(uint64_t virtual_addr, uint64_t physical_addr);
int kernel_mm_unmap_page(uint64_t virtual_addr);
int kernel_mm_get_memory_info(kernel_mm_state_t *state);

#endif
