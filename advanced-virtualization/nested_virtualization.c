#include "nested_virtualization.h"
#include <stdlib.h>
#include <string.h>

static NestedVirtualizationManager *global_nested_virt = NULL;

NestedVirtualizationManager *nested_virt_init(void)
{
    NestedVirtualizationManager *manager = (NestedVirtualizationManager *)malloc(sizeof(NestedVirtualizationManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(NestedVirtualizationManager));
    manager->manager_id = 1;
    manager->nesting_level = NESTING_L1;
    manager->nested_vm_count = 0;
    manager->total_vmexits = 0;
    manager->total_vmentries = 0;
    manager->ept_violations = 0;
    manager->ept_misconfigurations = 0;
    manager->vmexit_rate = 0.0f;
    manager->ept_violation_rate = 0.0f;

    global_nested_virt = manager;
    return manager;
}

int nested_virt_destroy(NestedVirtualizationManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_nested_virt = NULL;
    return 0;
}

int nested_vm_create(NestedVirtualizationManager *manager, uint32_t vm_id, uint32_t parent_vm_id, uint32_t depth)
{
    if (!manager || vm_id >= MAX_NESTED_VMS || depth >= MAX_NESTING_LEVELS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    memset(nvm, 0, sizeof(NestedVM));

    nvm->nested_vm_id = vm_id;
    nvm->parent_vm_id = parent_vm_id;
    nvm->nesting_depth = depth;
    nvm->nesting_level = (NestingLevel)depth;
    nvm->vcpu_count = 0;
    nvm->ept_table_count = 0;
    nvm->total_vmexits = 0;
    nvm->total_vmentries = 0;
    nvm->nested_vmexit_count = 0;

    for (uint32_t i = 0; i < MAX_EPT_LEVELS; i++)
    {
        nvm->ept_tables[i].ept_level = i;
        nvm->ept_tables[i].ept_pointer = (uint64_t)(vm_id * 1000 + i);
        nvm->ept_tables[i].entry_count = 0;
        nvm->ept_tables[i].mappings_total = 0;
    }

    manager->nested_vm_count++;
    return 0;
}

int nested_vm_destroy(NestedVirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager || vm_id >= MAX_NESTED_VMS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    if (nvm->nested_vm_id == 0)
        return -1;

    memset(nvm, 0, sizeof(NestedVM));
    manager->nested_vm_count--;

    return 0;
}

int ept_setup_table(NestedVirtualizationManager *manager, uint32_t vm_id, uint32_t level)
{
    if (!manager || vm_id >= MAX_NESTED_VMS || level >= MAX_EPT_LEVELS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    if (nvm->nested_vm_id == 0)
        return -1;

    EPTTable *table = &nvm->ept_tables[level];
    table->ept_level = level;
    table->ept_pointer = (uint64_t)(vm_id * 1000 + level);
    table->entry_count = 0;
    table->mappings_total = 0;

    nvm->ept_table_count++;
    return 0;
}

int ept_map_gpa_to_hpa(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa, uint64_t hpa)
{
    if (!manager || vm_id >= MAX_NESTED_VMS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    if (nvm->nested_vm_id == 0)
        return -1;

    EPTTable *table = &nvm->ept_tables[0];
    if (table->entry_count >= 4096)
        return -1;

    EPTEntry *entry = &table->entries[table->entry_count];
    entry->gpa = gpa;
    entry->hpa = hpa;
    entry->state = EPT_STATE_RESIDENT;
    entry->read_access = 1;
    entry->write_access = 1;
    entry->execute_access = 1;

    table->entry_count++;
    table->mappings_total++;

    return 0;
}

int ept_unmap_page(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa)
{
    if (!manager || vm_id >= MAX_NESTED_VMS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    if (nvm->nested_vm_id == 0)
        return -1;

    EPTTable *table = &nvm->ept_tables[0];

    for (uint32_t i = 0; i < table->entry_count; i++)
    {
        if (table->entries[i].gpa == gpa)
        {
            table->entries[i].state = EPT_STATE_UNMAPPED;
            return 0;
        }
    }

    return -1;
}

int ept_handle_violation(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa)
{
    if (!manager || vm_id >= MAX_NESTED_VMS)
        return -1;

    manager->ept_violations++;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    EPTTable *table = &nvm->ept_tables[0];

    for (uint32_t i = 0; i < table->entry_count; i++)
    {
        if (table->entries[i].gpa == gpa)
        {
            if (table->entries[i].state == EPT_STATE_UNMAPPED)
            {
                table->entries[i].state = EPT_STATE_RESIDENT;
                return 0;
            }
        }
    }

    return -1;
}

int ept_update_permissions(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa, uint8_t rwx)
{
    if (!manager || vm_id >= MAX_NESTED_VMS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    EPTTable *table = &nvm->ept_tables[0];

    for (uint32_t i = 0; i < table->entry_count; i++)
    {
        if (table->entries[i].gpa == gpa)
        {
            table->entries[i].read_access = (rwx >> 0) & 1;
            table->entries[i].write_access = (rwx >> 1) & 1;
            table->entries[i].execute_access = (rwx >> 2) & 1;
            return 0;
        }
    }

    return -1;
}

int nested_vmexit_handler(NestedVirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager || vm_id >= MAX_NESTED_VMS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    if (nvm->nested_vm_id == 0)
        return -1;

    nvm->total_vmexits++;
    manager->total_vmexits++;

    return 0;
}

int nested_vmentry_handler(NestedVirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager || vm_id >= MAX_NESTED_VMS)
        return -1;

    NestedVM *nvm = &manager->nested_vms[vm_id];
    if (nvm->nested_vm_id == 0)
        return -1;

    nvm->total_vmentries++;
    manager->total_vmentries++;

    return 0;
}

int nested_virt_get_metrics(NestedVirtualizationManager *manager, NestedVirtMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->nested_vm_count = manager->nested_vm_count;
    metrics->total_vmexits = manager->total_vmexits;
    metrics->total_vmentries = manager->total_vmentries;
    metrics->ept_violations = manager->ept_violations;

    if (manager->total_vmexits + manager->total_vmentries > 0)
    {
        metrics->vmexit_rate = (float)manager->total_vmexits / (manager->total_vmexits + manager->total_vmentries);
    }
    else
    {
        metrics->vmexit_rate = 0.0f;
    }

    return 0;
}
