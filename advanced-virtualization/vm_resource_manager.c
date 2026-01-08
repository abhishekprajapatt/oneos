#include "vm_resource_manager.h"
#include <stdlib.h>
#include <string.h>

static VMResourceManager *global_vm_resource_manager = NULL;

VMResourceManager *vm_resource_manager_init(uint32_t pool_count)
{
    VMResourceManager *manager = (VMResourceManager *)malloc(sizeof(VMResourceManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(VMResourceManager));
    manager->manager_id = 1;
    manager->pool_count = (pool_count > MAX_RESOURCE_POOLS) ? MAX_RESOURCE_POOLS : pool_count;
    manager->total_vms = 0;
    manager->total_vcpu_count = 0;
    manager->total_memory_capacity_mb = MAX_VM_MEMORY_MB;
    manager->total_memory_allocated_mb = 0;
    manager->cpu_utilization = 0.0f;
    manager->memory_utilization = 0.0f;
    manager->oversubscription_events = 0;

    for (uint32_t i = 0; i < manager->pool_count; i++)
    {
        manager->pools[i].pool_id = i;
        manager->pools[i].resource_type = (ResourceType)(i % 5);
        manager->pools[i].allocation_mode = ALLOC_DYNAMIC;
        manager->pools[i].total_capacity = (MAX_VM_MEMORY_MB / manager->pool_count);
        manager->pools[i].allocated_capacity = 0;
        manager->pools[i].available_capacity = manager->pools[i].total_capacity;
        manager->pools[i].vm_count = 0;
        manager->pools[i].reservation_count = 0;
        manager->pools[i].utilization_ratio = 0.0f;
        manager->pools[i].reservation_ratio = 0.0f;
    }

    global_vm_resource_manager = manager;
    return manager;
}

int vm_resource_manager_destroy(VMResourceManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_vm_resource_manager = NULL;
    return 0;
}

int vm_allocate_resources(VMResourceManager *manager, uint32_t vm_id, uint32_t vcpus, uint64_t memory_mb)
{
    if (!manager || vm_id >= MAX_VMS || vcpus == 0 || memory_mb == 0)
        return -1;

    if (manager->total_memory_allocated_mb + memory_mb > manager->total_memory_capacity_mb)
        return -1;

    for (uint32_t i = 0; i < vcpus; i++)
    {
        if (manager->total_vcpu_count >= MAX_VCPUS)
            return -1;

        vCPUInfo *vcpu = &manager->vcpus[manager->total_vcpu_count];
        vcpu->vcpu_id = manager->total_vcpu_count;
        vcpu->vm_id = vm_id;
        vcpu->cycles_executed = 0;
        vcpu->cpu_usage_percent = 0.0f;
        vcpu->core_affinity = manager->total_vcpu_count % 128;
        vcpu->is_active = 1;
        vcpu->is_oversubscribed = 0;

        manager->total_vcpu_count++;
    }

    VMMemoryState *vm_mem = &manager->vm_memory[vm_id];
    vm_mem->vm_id = vm_id;
    vm_mem->allocated_memory_mb = memory_mb;
    vm_mem->used_memory_mb = 0;
    vm_mem->peak_memory_mb = 0;
    vm_mem->page_count = (memory_mb * 256);
    vm_mem->dirty_pages = 0;
    vm_mem->shared_pages = 0;
    vm_mem->memory_utilization = 0.0f;
    vm_mem->is_ballooning = 0;

    manager->total_memory_allocated_mb += memory_mb;
    manager->total_vms++;

    ResourcePool *best_pool = NULL;
    for (uint32_t i = 0; i < manager->pool_count; i++)
    {
        if (manager->pools[i].available_capacity >= memory_mb)
        {
            if (!best_pool || manager->pools[i].utilization_ratio < best_pool->utilization_ratio)
            {
                best_pool = &manager->pools[i];
            }
        }
    }

    if (best_pool)
    {
        best_pool->allocated_capacity += memory_mb;
        best_pool->available_capacity -= memory_mb;
        best_pool->vm_count++;
        best_pool->utilization_ratio = (float)best_pool->allocated_capacity / best_pool->total_capacity;
    }

    return 0;
}

int vm_deallocate_resources(VMResourceManager *manager, uint32_t vm_id)
{
    if (!manager || vm_id >= MAX_VMS)
        return -1;

    VMMemoryState *vm_mem = &manager->vm_memory[vm_id];
    if (vm_mem->allocated_memory_mb == 0)
        return -1;

    manager->total_memory_allocated_mb -= vm_mem->allocated_memory_mb;

    for (uint32_t i = 0; i < manager->pool_count; i++)
    {
        if (manager->pools[i].allocated_capacity >= vm_mem->allocated_memory_mb)
        {
            manager->pools[i].allocated_capacity -= vm_mem->allocated_memory_mb;
            manager->pools[i].available_capacity += vm_mem->allocated_memory_mb;
            manager->pools[i].vm_count--;
            manager->pools[i].utilization_ratio = (float)manager->pools[i].allocated_capacity / manager->pools[i].total_capacity;
            break;
        }
    }

    uint32_t vcpu_removed = 0;
    for (uint32_t i = 0; i < manager->total_vcpu_count; i++)
    {
        if (manager->vcpus[i].vm_id == vm_id)
        {
            memset(&manager->vcpus[i], 0, sizeof(vCPUInfo));
            vcpu_removed++;
        }
    }

    memset(vm_mem, 0, sizeof(VMMemoryState));
    manager->total_vms--;

    return 0;
}

int vm_reserve_resources(VMResourceManager *manager, uint32_t vm_id, ResourceType type, uint64_t amount)
{
    if (!manager || vm_id >= MAX_VMS || amount == 0)
        return -1;

    for (uint32_t i = 0; i < manager->pool_count; i++)
    {
        if (manager->pools[i].resource_type == type && manager->pools[i].available_capacity >= amount)
        {
            manager->pools[i].available_capacity -= amount;
            manager->pools[i].reservation_count++;
            manager->pools[i].reservation_ratio = (float)manager->pools[i].reservation_count / manager->pools[i].vm_count;
            return 0;
        }
    }

    return -1;
}

int vm_release_reservation(VMResourceManager *manager, uint32_t vm_id, ResourceType type)
{
    if (!manager || vm_id >= MAX_VMS)
        return -1;

    for (uint32_t i = 0; i < manager->pool_count; i++)
    {
        if (manager->pools[i].resource_type == type && manager->pools[i].reservation_count > 0)
        {
            manager->pools[i].reservation_count--;
            manager->pools[i].reservation_ratio = (manager->pools[i].vm_count > 0) ? (float)manager->pools[i].reservation_count / manager->pools[i].vm_count : 0.0f;
            return 0;
        }
    }

    return -1;
}

int vm_rebalance_resources(VMResourceManager *manager)
{
    if (!manager)
        return -1;

    uint32_t rebalanced = 0;

    for (uint32_t i = 0; i < MAX_VMS; i++)
    {
        if (manager->vm_memory[i].allocated_memory_mb > 0)
        {
            float utilization = (manager->vm_memory[i].used_memory_mb > 0) ? (float)manager->vm_memory[i].used_memory_mb / manager->vm_memory[i].allocated_memory_mb : 0.0f;
            manager->vm_memory[i].memory_utilization = utilization;

            if (utilization > 0.85f || utilization < 0.15f)
            {
                rebalanced++;
            }
        }
    }

    return rebalanced;
}

int vm_hot_resize(VMResourceManager *manager, uint32_t vm_id, uint32_t new_vcpus, uint64_t new_memory_mb)
{
    if (!manager || vm_id >= MAX_VMS || new_vcpus == 0 || new_memory_mb == 0)
        return -1;

    VMMemoryState *vm_mem = &manager->vm_memory[vm_id];
    if (vm_mem->allocated_memory_mb == 0)
        return -1;

    int32_t memory_delta = new_memory_mb - vm_mem->allocated_memory_mb;
    if (memory_delta > 0 && manager->total_memory_allocated_mb + memory_delta > manager->total_memory_capacity_mb)
        return -1;

    vm_mem->allocated_memory_mb = new_memory_mb;
    vm_mem->page_count = (new_memory_mb * 256);
    manager->total_memory_allocated_mb += memory_delta;

    return 0;
}

int vm_enable_memory_ballooning(VMResourceManager *manager, uint32_t vm_id)
{
    if (!manager || vm_id >= MAX_VMS)
        return -1;

    VMMemoryState *vm_mem = &manager->vm_memory[vm_id];
    if (vm_mem->allocated_memory_mb == 0)
        return -1;

    vm_mem->is_ballooning = 1;
    return 0;
}

int vm_disable_memory_ballooning(VMResourceManager *manager, uint32_t vm_id)
{
    if (!manager || vm_id >= MAX_VMS)
        return -1;

    VMMemoryState *vm_mem = &manager->vm_memory[vm_id];
    vm_mem->is_ballooning = 0;
    return 0;
}

int vm_detect_oversubscription(VMResourceManager *manager)
{
    if (!manager)
        return 0;

    uint32_t oversubscribed = 0;

    for (uint32_t i = 0; i < manager->total_vcpu_count; i++)
    {
        if (manager->vcpus[i].cpu_usage_percent > 95.0f)
        {
            manager->vcpus[i].is_oversubscribed = 1;
            oversubscribed++;
        }
    }

    manager->oversubscription_events += oversubscribed;
    return oversubscribed;
}

int vm_handle_oversubscription(VMResourceManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->total_vcpu_count; i++)
    {
        if (manager->vcpus[i].is_oversubscribed)
        {
            manager->vcpus[i].cpu_usage_percent *= 0.9f;
            if (manager->vcpus[i].cpu_usage_percent < 80.0f)
            {
                manager->vcpus[i].is_oversubscribed = 0;
            }
        }
    }

    return 0;
}

int vm_get_resource_metrics(VMResourceManager *manager, ResourceMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    metrics->manager_id = manager->manager_id;
    metrics->total_vms = manager->total_vms;
    metrics->total_memory_allocated_mb = manager->total_memory_allocated_mb;
    metrics->oversubscription_count = manager->oversubscription_events;

    if (manager->total_memory_capacity_mb > 0)
    {
        metrics->memory_utilization = (float)manager->total_memory_allocated_mb / manager->total_memory_capacity_mb;
    }
    else
    {
        metrics->memory_utilization = 0.0f;
    }

    float total_cpu_usage = 0.0f;
    if (manager->total_vcpu_count > 0)
    {
        for (uint32_t i = 0; i < manager->total_vcpu_count; i++)
        {
            total_cpu_usage += manager->vcpus[i].cpu_usage_percent;
        }
        metrics->cpu_utilization = total_cpu_usage / manager->total_vcpu_count;
    }
    else
    {
        metrics->cpu_utilization = 0.0f;
    }

    return 0;
}
