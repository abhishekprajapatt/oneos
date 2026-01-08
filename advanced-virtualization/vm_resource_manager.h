#ifndef VM_RESOURCE_MANAGER_H
#define VM_RESOURCE_MANAGER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_VMS 256
#define MAX_VCPUS 1024
#define MAX_VM_MEMORY_MB 1048576
#define MAX_RESOURCE_POOLS 16

typedef enum
{
    RESOURCE_CPU,
    RESOURCE_MEMORY,
    RESOURCE_DISK,
    RESOURCE_NETWORK,
    RESOURCE_IO
} ResourceType;

typedef enum
{
    ALLOC_DEDICATED,
    ALLOC_SHARED,
    ALLOC_DYNAMIC
} AllocationMode;

typedef struct
{
    uint32_t vcpu_id;
    uint32_t vm_id;
    uint64_t cycles_executed;
    float cpu_usage_percent;

    uint32_t core_affinity;
    uint8_t is_active;
    uint8_t is_oversubscribed;
} vCPUInfo;

typedef struct
{
    uint32_t vm_id;
    uint64_t allocated_memory_mb;
    uint64_t used_memory_mb;
    uint64_t peak_memory_mb;

    uint32_t page_count;
    uint32_t dirty_pages;
    uint32_t shared_pages;

    float memory_utilization;
    uint8_t is_ballooning;
} VMMemoryState;

typedef struct
{
    uint32_t pool_id;
    ResourceType resource_type;
    AllocationMode allocation_mode;

    uint64_t total_capacity;
    uint64_t allocated_capacity;
    uint64_t available_capacity;

    uint32_t vm_count;
    uint32_t reservation_count;

    float utilization_ratio;
    float reservation_ratio;
} ResourcePool;

typedef struct
{
    uint32_t manager_id;
    uint32_t pool_count;
    ResourcePool pools[MAX_RESOURCE_POOLS];

    uint32_t total_vms;
    vCPUInfo vcpus[MAX_VCPUS];
    VMMemoryState vm_memory[MAX_VMS];

    uint32_t total_vcpu_count;
    uint64_t total_memory_capacity_mb;
    uint64_t total_memory_allocated_mb;

    float cpu_utilization;
    float memory_utilization;
    uint32_t oversubscription_events;
} VMResourceManager;

VMResourceManager *vm_resource_manager_init(uint32_t pool_count);
int vm_resource_manager_destroy(VMResourceManager *manager);

int vm_allocate_resources(VMResourceManager *manager, uint32_t vm_id, uint32_t vcpus, uint64_t memory_mb);
int vm_deallocate_resources(VMResourceManager *manager, uint32_t vm_id);

int vm_reserve_resources(VMResourceManager *manager, uint32_t vm_id, ResourceType type, uint64_t amount);
int vm_release_reservation(VMResourceManager *manager, uint32_t vm_id, ResourceType type);

int vm_rebalance_resources(VMResourceManager *manager);
int vm_hot_resize(VMResourceManager *manager, uint32_t vm_id, uint32_t new_vcpus, uint64_t new_memory_mb);

int vm_enable_memory_ballooning(VMResourceManager *manager, uint32_t vm_id);
int vm_disable_memory_ballooning(VMResourceManager *manager, uint32_t vm_id);

int vm_detect_oversubscription(VMResourceManager *manager);
int vm_handle_oversubscription(VMResourceManager *manager);

typedef struct
{
    uint32_t manager_id;
    float cpu_utilization;
    float memory_utilization;
    uint32_t oversubscription_count;
    uint32_t total_vms;
    uint64_t total_memory_allocated_mb;
} ResourceMetrics;

int vm_get_resource_metrics(VMResourceManager *manager, ResourceMetrics *metrics);

#endif
