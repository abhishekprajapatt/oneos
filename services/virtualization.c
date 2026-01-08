#include "virtualization.h"
#include <stdlib.h>
#include <string.h>

VirtualizationManager *virt_init(HypervisorType hypervisor, uint32_t host_cpus, uint64_t host_memory_mb)
{
    VirtualizationManager *manager = (VirtualizationManager *)malloc(sizeof(VirtualizationManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(VirtualizationManager));
    manager->active_hypervisor = hypervisor;
    manager->total_host_cpus = host_cpus;
    manager->total_host_memory_mb = host_memory_mb;
    manager->used_host_memory_mb = 0;
    manager->vm_count = 0;
    manager->migration_count = 0;
    manager->max_concurrent_vms = 256;
    manager->nested_virtualization_enabled = false;
    manager->total_vm_uptime_sec = 0;

    return manager;
}

void virt_cleanup(VirtualizationManager *manager)
{
    if (!manager)
        return;

    for (uint32_t i = 0; i < manager->vm_count; i++)
    {
        if (manager->vms[i].state == VM_STATE_RUNNING)
        {
            virt_stop_vm(manager, manager->vms[i].vm_id);
        }
    }

    memset(manager, 0, sizeof(VirtualizationManager));
    free(manager);
}

uint32_t virt_create_vm(VirtualizationManager *manager, const char *name,
                        uint32_t vcpu_count, uint32_t memory_mb)
{
    if (!manager || manager->vm_count >= MAX_VIRTUAL_MACHINES)
        return 0;
    if (!name || vcpu_count > MAX_VCPU_PER_VM || memory_mb > MAX_VM_MEMORY_MB)
        return 0;
    if (manager->used_host_memory_mb + memory_mb > manager->total_host_memory_mb)
        return 0;

    VirtualMachine *vm = &manager->vms[manager->vm_count];
    vm->vm_id = manager->vm_count + 1;
    strncpy(vm->name, name, 63);
    vm->name[63] = '\0';
    vm->state = VM_STATE_STOPPED;
    vm->hypervisor = manager->active_hypervisor;
    vm->vcpu_count = vcpu_count;
    vm->allocated_memory_mb = memory_mb;
    vm->max_memory_mb = memory_mb;
    vm->disk_count = 0;
    vm->virtual_nic_count = 0;
    vm->total_cycles = 0;
    vm->io_operations = 0;
    vm->created_time = time(NULL);
    vm->last_modified = time(NULL);

    for (uint32_t i = 0; i < vcpu_count; i++)
    {
        vm->vcpus[i].vcpu_id = i;
        vm->vcpus[i].cpu_affinity = i % manager->total_host_cpus;
        vm->vcpus[i].cycles_executed = 0;
        vm->vcpus[i].current_frequency_mhz = 2400;
        vm->vcpus[i].is_online = true;
    }

    manager->used_host_memory_mb += memory_mb;
    manager->vm_count++;

    return vm->vm_id;
}

int virt_delete_vm(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->vm_count; i++)
    {
        if (manager->vms[i].vm_id == vm_id)
        {
            if (manager->vms[i].state == VM_STATE_RUNNING)
            {
                virt_stop_vm(manager, vm_id);
            }
            manager->used_host_memory_mb -= manager->vms[i].allocated_memory_mb;
            memmove(&manager->vms[i], &manager->vms[i + 1],
                    (manager->vm_count - i - 1) * sizeof(VirtualMachine));
            manager->vm_count--;
            return 0;
        }
    }
    return -1;
}

int virt_start_vm(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->state != VM_STATE_STOPPED)
        return -1;

    vm->state = VM_STATE_RUNNING;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_stop_vm(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->state == VM_STATE_STOPPED)
        return -1;

    vm->state = VM_STATE_STOPPED;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_pause_vm(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->state != VM_STATE_RUNNING)
        return -1;

    vm->state = VM_STATE_PAUSED;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_resume_vm(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->state != VM_STATE_PAUSED)
        return -1;

    vm->state = VM_STATE_RUNNING;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_attach_disk(VirtualizationManager *manager, uint32_t vm_id, const char *disk_name,
                     uint64_t size_mb, const char *image_path)
{
    if (!manager || !disk_name || !image_path)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->disk_count >= 16)
        return -1;

    VirtualDisk *disk = &vm->disks[vm->disk_count];
    disk->vdisk_id = vm->disk_count + 1;
    strncpy(disk->name, disk_name, 63);
    disk->name[63] = '\0';
    disk->size_mb = size_mb;
    strncpy(disk->image_path, image_path, 255);
    disk->image_path[255] = '\0';
    disk->is_allocated = true;
    disk->io_operations = 0;

    vm->disk_count++;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_detach_disk(VirtualizationManager *manager, uint32_t vm_id, uint32_t disk_id)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm)
        return -1;

    for (uint32_t i = 0; i < vm->disk_count; i++)
    {
        if (vm->disks[i].vdisk_id == disk_id)
        {
            memmove(&vm->disks[i], &vm->disks[i + 1],
                    (vm->disk_count - i - 1) * sizeof(VirtualDisk));
            vm->disk_count--;
            vm->last_modified = time(NULL);
            return 0;
        }
    }
    return -1;
}

int virt_add_vcpu(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->vcpu_count >= MAX_VCPU_PER_VM)
        return -1;

    VCPU *vcpu = &vm->vcpus[vm->vcpu_count];
    vcpu->vcpu_id = vm->vcpu_count;
    vcpu->cpu_affinity = vm->vcpu_count % manager->total_host_cpus;
    vcpu->cycles_executed = 0;
    vcpu->current_frequency_mhz = 2400;
    vcpu->is_online = true;

    vm->vcpu_count++;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_remove_vcpu(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->vcpu_count <= 1)
        return -1;

    vm->vcpu_count--;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_allocate_memory(VirtualizationManager *manager, uint32_t vm_id, uint32_t memory_mb)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm)
        return -1;

    if (manager->used_host_memory_mb - vm->allocated_memory_mb +
            vm->allocated_memory_mb + memory_mb >
        manager->total_host_memory_mb)
    {
        return -1;
    }

    manager->used_host_memory_mb -= vm->allocated_memory_mb;
    vm->allocated_memory_mb += memory_mb;
    manager->used_host_memory_mb += vm->allocated_memory_mb;
    vm->last_modified = time(NULL);

    return 0;
}

int virt_deallocate_memory(VirtualizationManager *manager, uint32_t vm_id, uint32_t memory_mb)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm || vm->allocated_memory_mb < memory_mb)
        return -1;

    manager->used_host_memory_mb -= vm->allocated_memory_mb;
    vm->allocated_memory_mb -= memory_mb;
    manager->used_host_memory_mb += vm->allocated_memory_mb;
    vm->last_modified = time(NULL);

    return 0;
}

int virt_set_vcpu_affinity(VirtualizationManager *manager, uint32_t vm_id, uint32_t vcpu_id, uint32_t affinity)
{
    if (!manager)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm)
        return -1;

    VCPU *vcpu = virt_get_vcpu(vm, vcpu_id);
    if (!vcpu)
        return -1;

    vcpu->cpu_affinity = affinity % manager->total_host_cpus;
    vm->last_modified = time(NULL);
    return 0;
}

int virt_migrate_vm(VirtualizationManager *manager, uint32_t vm_id, uint32_t target_host_id)
{
    if (!manager || manager->migration_count >= MAX_VIRTUAL_MACHINES)
        return -1;

    VirtualMachine *vm = virt_get_vm(manager, vm_id);
    if (!vm)
        return -1;

    Migration *migration = &manager->migrations[manager->migration_count];
    migration->vm_id = vm_id;
    migration->source_state = vm->state;
    migration->target_state = VM_STATE_RUNNING;
    migration->progress_percent = 0;
    migration->start_time = time(NULL);
    migration->estimated_completion = migration->start_time + 30;
    migration->is_active = true;

    vm->state = VM_STATE_MIGRATING;
    manager->migration_count++;

    return 0;
}

int virt_get_migration_progress(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->migration_count; i++)
    {
        if (manager->migrations[i].vm_id == vm_id && manager->migrations[i].is_active)
        {
            return manager->migrations[i].progress_percent;
        }
    }
    return -1;
}

int virt_cancel_migration(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->migration_count; i++)
    {
        if (manager->migrations[i].vm_id == vm_id && manager->migrations[i].is_active)
        {
            manager->migrations[i].is_active = false;

            VirtualMachine *vm = virt_get_vm(manager, vm_id);
            if (vm)
            {
                vm->state = manager->migrations[i].source_state;
            }
            return 0;
        }
    }
    return -1;
}

VirtualMachine *virt_get_vm(VirtualizationManager *manager, uint32_t vm_id)
{
    if (!manager)
        return NULL;

    for (uint32_t i = 0; i < manager->vm_count; i++)
    {
        if (manager->vms[i].vm_id == vm_id)
        {
            return &manager->vms[i];
        }
    }
    return NULL;
}

VCPU *virt_get_vcpu(VirtualMachine *vm, uint32_t vcpu_id)
{
    if (!vm)
        return NULL;

    for (uint32_t i = 0; i < vm->vcpu_count; i++)
    {
        if (vm->vcpus[i].vcpu_id == vcpu_id)
        {
            return &vm->vcpus[i];
        }
    }
    return NULL;
}

VirtualDisk *virt_get_disk(VirtualMachine *vm, uint32_t disk_id)
{
    if (!vm)
        return NULL;

    for (uint32_t i = 0; i < vm->disk_count; i++)
    {
        if (vm->disks[i].vdisk_id == disk_id)
        {
            return &vm->disks[i];
        }
    }
    return NULL;
}

int virt_enable_nested_virtualization(VirtualizationManager *manager, bool enable)
{
    if (!manager)
        return -1;
    manager->nested_virtualization_enabled = enable;
    return 0;
}

int virt_set_max_concurrent_vms(VirtualizationManager *manager, uint32_t max_vms)
{
    if (!manager || max_vms == 0 || max_vms > MAX_VIRTUAL_MACHINES)
        return -1;
    manager->max_concurrent_vms = max_vms;
    return 0;
}

uint32_t virt_get_vm_count(VirtualizationManager *manager)
{
    if (!manager)
        return 0;
    return manager->vm_count;
}

uint64_t virt_get_used_host_memory(VirtualizationManager *manager)
{
    if (!manager)
        return 0;
    return manager->used_host_memory_mb;
}

uint64_t virt_get_available_host_memory(VirtualizationManager *manager)
{
    if (!manager)
        return 0;
    return manager->total_host_memory_mb - manager->used_host_memory_mb;
}
