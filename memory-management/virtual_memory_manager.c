#include "virtual_memory_manager.h"
#include <stdlib.h>
#include <string.h>

static VirtualMemoryManager *global_vm_manager = NULL;
static uint32_t next_page_table_id = 1;

VirtualMemoryManager *vm_manager_init(uint64_t physical_mem, uint64_t swap_space)
{
    VirtualMemoryManager *manager = (VirtualMemoryManager *)malloc(sizeof(VirtualMemoryManager));
    if (!manager)
        return NULL;

    manager->page_tables = (PageTable *)malloc(sizeof(PageTable) * MAX_PAGE_TABLES);
    if (!manager->page_tables)
    {
        free(manager);
        return NULL;
    }

    memset(manager, 0, sizeof(VirtualMemoryManager));
    memset(manager->page_tables, 0, sizeof(PageTable) * MAX_PAGE_TABLES);
    manager->vm_manager_id = 1;
    manager->page_table_count = 0;
    manager->total_physical_memory = physical_mem;
    manager->total_swap_space = swap_space;
    manager->total_virtual_memory = physical_mem + swap_space;
    manager->memory_mapped = 0;
    manager->memory_swapped = 0;
    manager->total_page_faults = 0;
    manager->tlb_hits = 0;
    manager->tlb_misses = 0;
    manager->page_fault_rate = 0.0f;
    manager->tlb_hit_rate = 0.0f;
    manager->memory_utilization = 0.0f;

    global_vm_manager = manager;
    return manager;
}

int vm_manager_destroy(VirtualMemoryManager *manager)
{
    if (!manager)
        return -1;

    if (manager->page_tables)
        free(manager->page_tables);
    free(manager);
    global_vm_manager = NULL;
    return 0;
}

PageTable *vm_create_page_table(VirtualMemoryManager *manager, uint32_t process_id)
{
    if (!manager || manager->page_table_count >= MAX_PAGE_TABLES)
        return NULL;

    PageTable *pt = &manager->page_tables[manager->page_table_count];
    memset(pt, 0, sizeof(PageTable));

    pt->entries = (PageTableEntry *)malloc(sizeof(PageTableEntry) * MAX_PAGES);
    if (!pt->entries)
        return NULL;

    memset(pt->entries, 0, sizeof(PageTableEntry) * MAX_PAGES);
    pt->page_table_id = next_page_table_id++;
    pt->process_id = process_id;
    pt->entry_count = 0;
    pt->virtual_memory_used = 0;
    pt->resident_pages = 0;
    pt->swapped_pages = 0;
    pt->page_faults = 0;

    manager->page_table_count++;
    return pt;
}

int vm_destroy_page_table(VirtualMemoryManager *manager, uint32_t page_table_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->page_table_count; i++)
    {
        if (manager->page_tables[i].page_table_id == page_table_id)
        {
            if (manager->page_tables[i].entries)
                free(manager->page_tables[i].entries);
            memset(&manager->page_tables[i], 0, sizeof(PageTable));
            return 0;
        }
    }

    return -1;
}

PageTableEntry *vm_allocate_virtual_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->page_table_count; i++)
    {
        if (manager->page_tables[i].page_table_id == page_table_id)
        {
            PageTable *pt = &manager->page_tables[i];
            if (pt->entry_count >= MAX_PAGES)
                return NULL;

            PageTableEntry *entry = &pt->entries[pt->entry_count];
            memset(entry, 0, sizeof(PageTableEntry));

            entry->page_number = page_num;
            entry->physical_address = page_num * PAGE_SIZE;
            entry->state = PAGE_STATE_RESIDENT;
            entry->is_readable = 1;
            entry->is_writable = 1;
            entry->is_executable = 0;
            entry->access_count = 0;
            entry->last_access_time = 0;

            pt->entry_count++;
            pt->virtual_memory_used += PAGE_SIZE;
            pt->resident_pages++;
            manager->memory_mapped += PAGE_SIZE;

            return entry;
        }
    }

    return NULL;
}

int vm_free_virtual_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->page_table_count; i++)
    {
        if (manager->page_tables[i].page_table_id == page_table_id)
        {
            PageTable *pt = &manager->page_tables[i];

            for (uint32_t j = 0; j < pt->entry_count; j++)
            {
                if (pt->entries[j].page_number == page_num)
                {
                    pt->virtual_memory_used -= PAGE_SIZE;
                    if (pt->entries[j].state == PAGE_STATE_RESIDENT)
                    {
                        pt->resident_pages--;
                        manager->memory_mapped -= PAGE_SIZE;
                    }
                    else if (pt->entries[j].state == PAGE_STATE_SWAPPED)
                    {
                        pt->swapped_pages--;
                        manager->memory_swapped -= PAGE_SIZE;
                    }
                    memset(&pt->entries[j], 0, sizeof(PageTableEntry));
                    return 0;
                }
            }
        }
    }

    return -1;
}

int vm_page_fault_handler(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->page_table_count; i++)
    {
        if (manager->page_tables[i].page_table_id == page_table_id)
        {
            PageTable *pt = &manager->page_tables[i];
            pt->page_faults++;
            manager->total_page_faults++;

            for (uint32_t j = 0; j < pt->entry_count; j++)
            {
                if (pt->entries[j].page_number == page_num)
                {
                    if (pt->entries[j].state == PAGE_STATE_SWAPPED)
                    {
                        return vm_swap_page_in(manager, &pt->entries[j]);
                    }
                    else if (pt->entries[j].state == PAGE_STATE_UNMAPPED)
                    {
                        pt->entries[j].state = PAGE_STATE_RESIDENT;
                        pt->resident_pages++;
                        manager->memory_mapped += PAGE_SIZE;
                        return 0;
                    }
                }
            }
        }
    }

    return -1;
}

int vm_swap_page_out(VirtualMemoryManager *manager, PageTableEntry *entry)
{
    if (!manager || !entry)
        return -1;

    if (entry->state == PAGE_STATE_LOCKED || entry->state == PAGE_STATE_PINNED)
        return -1;

    entry->state = PAGE_STATE_SWAPPED;
    entry->swap_offset = manager->memory_swapped;
    manager->memory_swapped += PAGE_SIZE;

    return 0;
}

int vm_swap_page_in(VirtualMemoryManager *manager, PageTableEntry *entry)
{
    if (!manager || !entry)
        return -1;

    if (entry->state != PAGE_STATE_SWAPPED)
        return -1;

    entry->state = PAGE_STATE_RESIDENT;
    manager->memory_swapped -= PAGE_SIZE;

    return 0;
}

int vm_set_page_permissions(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num, uint8_t read, uint8_t write, uint8_t exec)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->page_table_count; i++)
    {
        if (manager->page_tables[i].page_table_id == page_table_id)
        {
            PageTable *pt = &manager->page_tables[i];

            for (uint32_t j = 0; j < pt->entry_count; j++)
            {
                if (pt->entries[j].page_number == page_num)
                {
                    pt->entries[j].is_readable = read;
                    pt->entries[j].is_writable = write;
                    pt->entries[j].is_executable = exec;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int vm_lock_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->page_table_count; i++)
    {
        if (manager->page_tables[i].page_table_id == page_table_id)
        {
            PageTable *pt = &manager->page_tables[i];

            for (uint32_t j = 0; j < pt->entry_count; j++)
            {
                if (pt->entries[j].page_number == page_num)
                {
                    pt->entries[j].state = PAGE_STATE_LOCKED;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int vm_unlock_page(VirtualMemoryManager *manager, uint32_t page_table_id, uint32_t page_num)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->page_table_count; i++)
    {
        if (manager->page_tables[i].page_table_id == page_table_id)
        {
            PageTable *pt = &manager->page_tables[i];

            for (uint32_t j = 0; j < pt->entry_count; j++)
            {
                if (pt->entries[j].page_number == page_num)
                {
                    pt->entries[j].state = PAGE_STATE_RESIDENT;
                    return 0;
                }
            }
        }
    }

    return -1;
}

int vm_invalidate_tlb(VirtualMemoryManager *manager, uint32_t page_table_id)
{
    if (!manager)
        return -1;

    return 0;
}

int vm_flush_tlb_entry(VirtualMemoryManager *manager, uint32_t virtual_address)
{
    if (!manager)
        return -1;

    return 0;
}

int vm_get_metrics(VirtualMemoryManager *manager, VMMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->vm_manager_id;
    metrics->total_memory = manager->total_virtual_memory;
    metrics->used_memory = manager->memory_mapped + manager->memory_swapped;
    metrics->swapped_memory = manager->memory_swapped;
    metrics->total_page_faults = manager->total_page_faults;

    if (manager->tlb_hits + manager->tlb_misses > 0)
    {
        metrics->tlb_hit_rate = (float)manager->tlb_hits / (manager->tlb_hits + manager->tlb_misses);
    }
    else
    {
        metrics->tlb_hit_rate = 0.0f;
    }

    if (manager->total_virtual_memory > 0)
    {
        metrics->page_fault_rate = (float)manager->total_page_faults / (manager->total_virtual_memory / PAGE_SIZE);
    }
    else
    {
        metrics->page_fault_rate = 0.0f;
    }

    return 0;
}
