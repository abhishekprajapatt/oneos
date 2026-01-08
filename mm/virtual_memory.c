#include "virtual_memory.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

VirtualMemoryManager *vm_init(uint64_t physical_memory_size)
{
    VirtualMemoryManager *vmm = (VirtualMemoryManager *)malloc(sizeof(VirtualMemoryManager));
    if (!vmm)
        return NULL;

    vmm->max_page_tables = MAX_PAGE_TABLES;
    vmm->page_table_count = 0;
    vmm->total_physical_memory = physical_memory_size;
    vmm->used_physical_memory = 0;
    vmm->page_faults = 0;
    vmm->swaps = 0;
    vmm->swap_size = physical_memory_size * 2;

    vmm->page_tables = (PageTable *)malloc(MAX_PAGE_TABLES * sizeof(PageTable));
    if (!vmm->page_tables)
    {
        free(vmm);
        return NULL;
    }

    vmm->physical_memory = (uint8_t *)malloc(physical_memory_size);
    if (!vmm->physical_memory)
    {
        free(vmm->page_tables);
        free(vmm);
        return NULL;
    }

    vmm->swap_storage = malloc(vmm->swap_size);
    if (!vmm->swap_storage)
    {
        free(vmm->physical_memory);
        free(vmm->page_tables);
        free(vmm);
        return NULL;
    }

    memset(vmm->page_tables, 0, MAX_PAGE_TABLES * sizeof(PageTable));
    memset(vmm->physical_memory, 0, physical_memory_size);

    return vmm;
}

void vm_cleanup(VirtualMemoryManager *vmm)
{
    if (!vmm)
        return;

    for (uint32_t i = 0; i < vmm->page_table_count; i++)
    {
        if (vmm->page_tables[i].entries)
        {
            free(vmm->page_tables[i].entries);
        }
    }

    if (vmm->page_tables)
        free(vmm->page_tables);
    if (vmm->physical_memory)
        free(vmm->physical_memory);
    if (vmm->swap_storage)
        free(vmm->swap_storage);

    free(vmm);
}

int vm_create_page_table(VirtualMemoryManager *vmm, uint64_t virtual_start, uint64_t virtual_end)
{
    if (!vmm || vmm->page_table_count >= MAX_PAGE_TABLES)
        return -1;

    PageTable *pt = &vmm->page_tables[vmm->page_table_count];
    pt->page_table_id = vmm->page_table_count + 1;
    pt->virtual_start = virtual_start;
    pt->virtual_end = virtual_end;
    pt->max_entries = (virtual_end - virtual_start) / PAGE_SIZE + 1;
    pt->entry_count = 0;

    pt->entries = (PageTableEntry *)malloc(pt->max_entries * sizeof(PageTableEntry));
    if (!pt->entries)
        return -1;

    memset(pt->entries, 0, pt->max_entries * sizeof(PageTableEntry));

    vmm->page_table_count++;
    return pt->page_table_id;
}

uint32_t vm_allocate_page(VirtualMemoryManager *vmm, uint32_t page_table_id)
{
    if (!vmm || page_table_id == 0 || page_table_id > vmm->page_table_count)
        return 0;

    PageTable *pt = &vmm->page_tables[page_table_id - 1];
    if (pt->entry_count >= pt->max_entries)
        return 0;

    PageTableEntry *pte = &pt->entries[pt->entry_count];
    pte->page_number = pt->entry_count;
    pte->frame_number = 0;
    pte->state = PAGE_ABSENT;
    pte->accessed = false;
    pte->dirty = false;
    pte->writable = true;
    pte->last_accessed = time(NULL);

    pt->entry_count++;
    return pte->page_number;
}

int vm_free_page(VirtualMemoryManager *vmm, uint32_t page_table_id, uint32_t page_number)
{
    if (!vmm || page_table_id == 0 || page_table_id > vmm->page_table_count)
        return -1;

    PageTable *pt = &vmm->page_tables[page_table_id - 1];
    if (page_number >= pt->entry_count)
        return -1;

    pt->entries[page_number].state = PAGE_ABSENT;
    pt->entries[page_number].frame_number = 0;

    return 0;
}

int vm_map_virtual_to_physical(VirtualMemoryManager *vmm, uint32_t page_table_id,
                               uint32_t virtual_page, uint32_t physical_frame)
{
    if (!vmm || page_table_id == 0 || page_table_id > vmm->page_table_count)
        return -1;

    PageTable *pt = &vmm->page_tables[page_table_id - 1];
    if (virtual_page >= pt->entry_count)
        return -1;

    pt->entries[virtual_page].frame_number = physical_frame;
    pt->entries[virtual_page].state = PAGE_PRESENT;

    return 0;
}

int vm_handle_page_fault(VirtualMemoryManager *vmm, uint32_t page_table_id, uint32_t page_number)
{
    if (!vmm || page_table_id == 0 || page_table_id > vmm->page_table_count)
        return -1;

    PageTable *pt = &vmm->page_tables[page_table_id - 1];
    if (page_number >= pt->entry_count)
        return -1;

    vmm->page_faults++;
    pt->entries[page_number].state = PAGE_PRESENT;
    pt->entries[page_number].accessed = true;
    pt->entries[page_number].last_accessed = time(NULL);

    return 0;
}

int vm_swap_page(VirtualMemoryManager *vmm, uint32_t page_table_id, uint32_t page_number)
{
    if (!vmm || page_table_id == 0 || page_table_id > vmm->page_table_count)
        return -1;

    PageTable *pt = &vmm->page_tables[page_table_id - 1];
    if (page_number >= pt->entry_count)
        return -1;

    pt->entries[page_number].state = PAGE_SWAPPED;
    vmm->swaps++;

    return 0;
}

uint64_t vm_get_available_memory(VirtualMemoryManager *vmm)
{
    if (!vmm)
        return 0;
    return vmm->total_physical_memory - vmm->used_physical_memory;
}

uint32_t vm_get_page_fault_count(VirtualMemoryManager *vmm)
{
    if (!vmm)
        return 0;
    return vmm->page_faults;
}

int vm_set_page_protection(VirtualMemoryManager *vmm, uint32_t page_table_id,
                           uint32_t page_number, bool writable)
{
    if (!vmm || page_table_id == 0 || page_table_id > vmm->page_table_count)
        return -1;

    PageTable *pt = &vmm->page_tables[page_table_id - 1];
    if (page_number >= pt->entry_count)
        return -1;

    pt->entries[page_number].writable = writable;

    return 0;
}
