#ifndef NESTED_VIRTUALIZATION_H
#define NESTED_VIRTUALIZATION_H

#include <stdint.h>
#include <stddef.h>

#define MAX_NESTING_LEVELS 4
#define MAX_NESTED_VMS 512
#define MAX_EPT_LEVELS 4

typedef enum
{
    NESTING_L0,
    NESTING_L1,
    NESTING_L2,
    NESTING_L3
} NestingLevel;

typedef enum
{
    EPT_STATE_UNMAPPED,
    EPT_STATE_RESIDENT,
    EPT_STATE_SWAPPED,
    EPT_STATE_SHARED
} EPTState;

typedef struct
{
    uint64_t gpa;
    uint64_t hpa;
    EPTState state;
    uint8_t read_access;
    uint8_t write_access;
    uint8_t execute_access;
} EPTEntry;

typedef struct
{
    uint32_t ept_level;
    uint64_t ept_pointer;
    EPTEntry entries[4096];
    uint32_t entry_count;
    uint64_t mappings_total;
} EPTTable;

typedef struct
{
    uint32_t nested_vm_id;
    uint32_t parent_vm_id;
    NestingLevel nesting_level;
    uint32_t nesting_depth;

    EPTTable ept_tables[MAX_EPT_LEVELS];
    uint32_t ept_table_count;

    uint64_t vcpu_states[64];
    uint32_t vcpu_count;

    uint8_t nested_vmexit_count;
    uint64_t total_vmexits;
    uint64_t total_vmentries;
} NestedVM;

typedef struct
{
    uint32_t manager_id;
    uint32_t nesting_level;
    uint32_t nested_vm_count;
    NestedVM nested_vms[MAX_NESTED_VMS];

    uint64_t total_vmexits;
    uint64_t total_vmentries;
    uint64_t ept_violations;
    uint64_t ept_misconfigurations;

    float vmexit_rate;
    float ept_violation_rate;
} NestedVirtualizationManager;

NestedVirtualizationManager *nested_virt_init(void);
int nested_virt_destroy(NestedVirtualizationManager *manager);

int nested_vm_create(NestedVirtualizationManager *manager, uint32_t vm_id, uint32_t parent_vm_id, uint32_t depth);
int nested_vm_destroy(NestedVirtualizationManager *manager, uint32_t vm_id);

int ept_setup_table(NestedVirtualizationManager *manager, uint32_t vm_id, uint32_t level);
int ept_map_gpa_to_hpa(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa, uint64_t hpa);
int ept_unmap_page(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa);

int ept_handle_violation(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa);
int ept_update_permissions(NestedVirtualizationManager *manager, uint32_t vm_id, uint64_t gpa, uint8_t rwx);

int nested_vmexit_handler(NestedVirtualizationManager *manager, uint32_t vm_id);
int nested_vmentry_handler(NestedVirtualizationManager *manager, uint32_t vm_id);

typedef struct
{
    uint32_t manager_id;
    uint32_t nested_vm_count;
    uint64_t total_vmexits;
    uint64_t total_vmentries;
    uint64_t ept_violations;
    float vmexit_rate;
} NestedVirtMetrics;

int nested_virt_get_metrics(NestedVirtualizationManager *manager, NestedVirtMetrics *metrics);

#endif
