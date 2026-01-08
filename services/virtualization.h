#ifndef VIRTUALIZATION_H
#define VIRTUALIZATION_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_VIRTUAL_MACHINES 256
#define MAX_VCPU_PER_VM 16
#define MAX_VM_MEMORY_MB 65536
#define MAX_VIRTUAL_DISKS 512

typedef enum
{
    VM_STATE_STOPPED = 0,
    VM_STATE_RUNNING = 1,
    VM_STATE_PAUSED = 2,
    VM_STATE_SUSPENDED = 3,
    VM_STATE_MIGRATING = 4
} VMState;

typedef enum
{
    HYPERVISOR_KVM = 0,
    HYPERVISOR_XEN = 1,
    HYPERVISOR_HYPER_V = 2,
    HYPERVISOR_QEMU = 3
} HypervisorType;

typedef struct
{
    uint32_t vcpu_id;
    uint32_t cpu_affinity;
    uint64_t cycles_executed;
    uint32_t current_frequency_mhz;
    bool is_online;
} VCPU;

typedef struct
{
    uint32_t vdisk_id;
    char name[64];
    uint64_t size_mb;
    char image_path[256];
    bool is_allocated;
    uint32_t io_operations;
} VirtualDisk;

typedef struct
{
    uint32_t vm_id;
    char name[64];
    VMState state;
    HypervisorType hypervisor;
    uint32_t vcpu_count;
    VCPU vcpus[MAX_VCPU_PER_VM];
    uint32_t allocated_memory_mb;
    uint32_t max_memory_mb;
    VirtualDisk disks[16];
    uint32_t disk_count;
    uint32_t virtual_nic_count;
    uint64_t total_cycles;
    uint64_t io_operations;
    time_t created_time;
    time_t last_modified;
} VirtualMachine;

typedef struct
{
    uint32_t vm_id;
    VMState source_state;
    VMState target_state;
    uint32_t progress_percent;
    time_t start_time;
    time_t estimated_completion;
    bool is_active;
} Migration;

typedef struct
{
    VirtualMachine vms[MAX_VIRTUAL_MACHINES];
    uint32_t vm_count;
    Migration migrations[MAX_VIRTUAL_MACHINES];
    uint32_t migration_count;
    HypervisorType active_hypervisor;
    uint32_t total_host_cpus;
    uint64_t total_host_memory_mb;
    uint64_t used_host_memory_mb;
    uint32_t max_concurrent_vms;
    bool nested_virtualization_enabled;
    uint64_t total_vm_uptime_sec;
} VirtualizationManager;

VirtualizationManager *virt_init(HypervisorType hypervisor, uint32_t host_cpus, uint64_t host_memory_mb);
void virt_cleanup(VirtualizationManager *manager);

uint32_t virt_create_vm(VirtualizationManager *manager, const char *name,
                        uint32_t vcpu_count, uint32_t memory_mb);
int virt_delete_vm(VirtualizationManager *manager, uint32_t vm_id);
int virt_start_vm(VirtualizationManager *manager, uint32_t vm_id);
int virt_stop_vm(VirtualizationManager *manager, uint32_t vm_id);
int virt_pause_vm(VirtualizationManager *manager, uint32_t vm_id);
int virt_resume_vm(VirtualizationManager *manager, uint32_t vm_id);

int virt_attach_disk(VirtualizationManager *manager, uint32_t vm_id, const char *disk_name,
                     uint64_t size_mb, const char *image_path);
int virt_detach_disk(VirtualizationManager *manager, uint32_t vm_id, uint32_t disk_id);
int virt_add_vcpu(VirtualizationManager *manager, uint32_t vm_id);
int virt_remove_vcpu(VirtualizationManager *manager, uint32_t vm_id);

int virt_allocate_memory(VirtualizationManager *manager, uint32_t vm_id, uint32_t memory_mb);
int virt_deallocate_memory(VirtualizationManager *manager, uint32_t vm_id, uint32_t memory_mb);
int virt_set_vcpu_affinity(VirtualizationManager *manager, uint32_t vm_id, uint32_t vcpu_id, uint32_t affinity);

int virt_migrate_vm(VirtualizationManager *manager, uint32_t vm_id, uint32_t target_host_id);
int virt_get_migration_progress(VirtualizationManager *manager, uint32_t vm_id);
int virt_cancel_migration(VirtualizationManager *manager, uint32_t vm_id);

VirtualMachine *virt_get_vm(VirtualizationManager *manager, uint32_t vm_id);
VCPU *virt_get_vcpu(VirtualMachine *vm, uint32_t vcpu_id);
VirtualDisk *virt_get_disk(VirtualMachine *vm, uint32_t disk_id);

int virt_enable_nested_virtualization(VirtualizationManager *manager, bool enable);
int virt_set_max_concurrent_vms(VirtualizationManager *manager, uint32_t max_vms);

uint32_t virt_get_vm_count(VirtualizationManager *manager);
uint64_t virt_get_used_host_memory(VirtualizationManager *manager);
uint64_t virt_get_available_host_memory(VirtualizationManager *manager);

#endif
