#ifndef VIRTUAL_MEMORY_H
#define VIRTUAL_MEMORY_H

#include <stdint.h>
#include <stdbool.h>

#define PAGE_SIZE 4096
#define MAX_PAGES 1000000
#define MAX_PAGE_TABLES 65536

typedef enum
{
    PAGE_PRESENT,
    PAGE_ABSENT,
    PAGE_SWAPPED,
    PAGE_PROTECTED,
    PAGE_READONLY
} PageState;

typedef struct
{
    uint32_t page_number;
    uint32_t frame_number;
    PageState state;
    bool accessed;
    bool dirty;
    bool writable;
    time_t last_accessed;
} PageTableEntry;

typedef struct
{
    uint32_t page_table_id;
    PageTableEntry *entries;
    uint32_t entry_count;
    uint32_t max_entries;
    uint64_t virtual_start;
    uint64_t virtual_end;
} PageTable;

typedef struct
{
    PageTable *page_tables;
    uint32_t page_table_count;
    uint32_t max_page_tables;
    uint8_t *physical_memory;
    uint64_t total_physical_memory;
    uint64_t used_physical_memory;
    uint32_t page_faults;
    uint32_t swaps;
    void *swap_storage;
    uint64_t swap_size;
} VirtualMemoryManager;

VirtualMemoryManager *vm_init(uint64_t physical_memory_size);
void vm_cleanup(VirtualMemoryManager *vmm);
int vm_create_page_table(VirtualMemoryManager *vmm, uint64_t virtual_start, uint64_t virtual_end);
uint32_t vm_allocate_page(VirtualMemoryManager *vmm, uint32_t page_table_id);
int vm_free_page(VirtualMemoryManager *vmm, uint32_t page_table_id, uint32_t page_number);
int vm_map_virtual_to_physical(VirtualMemoryManager *vmm, uint32_t page_table_id,
                               uint32_t virtual_page, uint32_t physical_frame);
int vm_handle_page_fault(VirtualMemoryManager *vmm, uint32_t page_table_id, uint32_t page_number);
int vm_swap_page(VirtualMemoryManager *vmm, uint32_t page_table_id, uint32_t page_number);
uint64_t vm_get_available_memory(VirtualMemoryManager *vmm);
uint32_t vm_get_page_fault_count(VirtualMemoryManager *vmm);
int vm_set_page_protection(VirtualMemoryManager *vmm, uint32_t page_table_id,
                           uint32_t page_number, bool writable);

#endif
