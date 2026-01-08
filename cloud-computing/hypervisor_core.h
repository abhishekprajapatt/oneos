#ifndef HYPERVISOR_CORE_H
#define HYPERVISOR_CORE_H

#include <stdint.h>
#include <stddef.h>

#define MAX_VMS 64
#define MAX_CPUS_PER_VM 16
#define MAX_MEMORY_PER_VM 0x80000000 // 2GB per VM

typedef enum
{
    VMM_QEMU,
    VMM_KVM,
    VMM_XEN,
    VMM_HYPER_V,
    VMM_CUSTOM
} HypervisorType;

typedef enum
{
    VM_STATE_STOPPED,
    VM_STATE_RUNNING,
    VM_STATE_PAUSED,
    VM_STATE_SUSPENDED,
    VM_STATE_CRASHED
} VMState;

typedef struct
{
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    uint64_t rip, rflags;
    uint64_t cr0, cr3, cr4;
} VCPUContext;

typedef struct
{
    uint32_t vcpu_id;
    VCPUContext context;
    uint64_t execution_time;
    uint32_t exit_count;
    uint32_t idle_time;
} VCPU;

typedef struct
{
    uint32_t vm_id;
    char name[64];
    VMState state;
    HypervisorType hypervisor_type;

    uint32_t vcpu_count;
    VCPU vcpus[MAX_CPUS_PER_VM];

    uint64_t memory_size;
    void *memory_base;

    uint64_t disk_size;
    void *disk_base;

    uint32_t network_interface_count;
    uint32_t created_timestamp;
    uint32_t uptime;
} VirtualMachine;

typedef struct
{
    uint32_t hypervisor_id;
    HypervisorType type;

    uint32_t vm_count;
    VirtualMachine vms[MAX_VMS];

    uint64_t total_memory_available;
    uint64_t total_memory_used;

    uint32_t total_vcpus_available;
    uint32_t total_vcpus_used;

    uint32_t active_vms;
} Hypervisor;

// Hypervisor initialization and management
Hypervisor *hypervisor_init(HypervisorType type);
int hypervisor_destroy(Hypervisor *hv);
int hypervisor_get_stats(Hypervisor *hv);

// VM lifecycle management
VirtualMachine *vm_create(Hypervisor *hv, const char *name, uint64_t memory_size, uint32_t vcpu_count);
int vm_start(VirtualMachine *vm);
int vm_stop(VirtualMachine *vm);
int vm_pause(VirtualMachine *vm);
int vm_resume(VirtualMachine *vm);
int vm_delete(Hypervisor *hv, uint32_t vm_id);

// VCPU management
int vcpu_create(VirtualMachine *vm, uint32_t vcpu_id);
int vcpu_run(VCPU *vcpu);
int vcpu_exit_handler(VCPU *vcpu);
int vcpu_save_context(VCPU *vcpu);
int vcpu_restore_context(VCPU *vcpu);

// Memory management for VMs
int vm_allocate_memory(VirtualMachine *vm, uint64_t size);
int vm_free_memory(VirtualMachine *vm);
int vm_map_memory(VirtualMachine *vm, uint64_t gpa, uint64_t hpa, uint64_t size);

// Resource monitoring
typedef struct
{
    uint32_t vm_id;
    float cpu_usage_percent;
    uint64_t memory_used;
    uint64_t disk_io_read;
    uint64_t disk_io_write;
    uint64_t network_bytes_in;
    uint64_t network_bytes_out;
} VMMetrics;

int vm_get_metrics(VirtualMachine *vm, VMMetrics *metrics);
int hypervisor_get_metrics(Hypervisor *hv);

#endif // HYPERVISOR_CORE_H
