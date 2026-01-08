#ifndef VIRTUAL_MEMORY_MANAGER_H
#define VIRTUAL_MEMORY_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_PAGE_TABLES 256
#define MAX_PAGES 16384
#define PAGE_SIZE 4096
#define MAX_SWAP_PAGES 32768

typedef enum
{
    PAGE_STATE_UNMAPPED,
    PAGE_STATE_RESIDENT,
    PAGE_STATE_SWAPPED,
    PAGE_STATE_PINNED,
    PAGE_STATE_LOCKED,
    PAGE_STATE_DIRTY
} PageState;

typedef struct
{
    uint32_t page_number;
    uint64_t physical_address;
    PageState state;
    uint8_t is_readable;
    uint8_t is_writable;
    uint8_t is_executable;
    uint16_t access_count;
    uint64_t last_access_time;
    uint64_t swap_offset;
} PageTableEntry;

typedef struct
{
    uint32_t page_table_id;
    uint32_t process_id;
    uint16_t entry_count;
    PageTableEntry *entries;
    uint64_t virtual_memory_used;
    uint64_t resident_pages;
    uint64_t swapped_pages;
    uint32_t page_faults;
} PageTable;

typedef struct
{
    uint32_t vm_manager_id;
    uint32_t page_table_count;
    PageTable *page_tables;

    uint64_t total_virtual_memory;
    uint64_t total_physical_memory;
    uint64_t total_swap_space;

    uint64_t memory_mapped;
    uint64_t memory_swapped;

    uint64_t total_page_faults;
    uint64_t tlb_hits;
    uint64_t tlb_misses;

    float page_fault_rate;
    float tlb_hit_rate;
    float memory_utilization;
} VirtualMemoryManager;

VirtualMemoryManager *vm_manager_init(uint64_t physical_mem, uint64_t swap_space);
int vm_manager_destroy(VirtualMemoryManager *manager);

PageTable *vm_create_page_table(VirtualMemoryManager *manager, uint32_t process_id);
int vm_destroy_page_table(VirtualMemoryManager *manager, uint32_t page_table_id);

PageTableEntry *vm_allocate_virtual_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num);
int vm_free_virtual_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num);

int vm_page_fault_handler(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num);
int vm_swap_page_out(VirtualMemoryManager *manager, PageTableEntry *entry);
int vm_swap_page_in(VirtualMemoryManager *manager, PageTableEntry *entry);

int vm_set_page_permissions(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num, uint8_t read, uint8_t write, uint8_t exec);
int vm_lock_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num);
int vm_unlock_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num);

int vm_invalidate_tlb(VirtualMemoryManager *manager, uint32_t page_table_id);
int vm_flush_tlb_entry(VirtualMemoryManager *manager, uint32_t virtual_address);

typedef struct
{
    uint32_t manager_id;
    uint64_t total_memory;
    uint64_t used_memory;
    uint64_t swapped_memory;
    uint64_t total_page_faults;
    float page_fault_rate;
    float tlb_hit_rate;
} VMMetrics;

int vm_get_metrics(VirtualMemoryManager *manager, VMMetrics *metrics);

#endif
