#ifndef ARCH_X86_MM_H
#define ARCH_X86_MM_H

#include <stdint.h>

typedef struct
{
    uint32_t virt_addr;
    uint32_t phys_addr;
    uint32_t size;
    uint32_t flags;
    uint32_t mapped;
} x86_page_entry_t;

typedef struct
{
    uint32_t total_pages;
    uint32_t mapped_pages;
    uint32_t free_pages;
    uint64_t total_memory;
    uint64_t free_memory;
} x86_memory_stats_t;

int x86_mm_init(void);
int x86_mm_setup_page_tables(uint32_t kernel_base, uint32_t kernel_size);
int x86_mm_allocate_page(uint32_t *phys_addr);
int x86_mm_free_page(uint32_t phys_addr);
int x86_mm_map_page(uint32_t virt_addr, uint32_t phys_addr, uint32_t flags);
int x86_mm_unmap_page(uint32_t virt_addr);
int x86_mm_get_stats(x86_memory_stats_t *stats);

#endif
