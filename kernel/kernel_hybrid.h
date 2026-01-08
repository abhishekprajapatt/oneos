#ifndef KERNEL_HYBRID_H
#define KERNEL_HYBRID_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
    KERNEL_LINUX,
    KERNEL_UNIX,
    KERNEL_HYBRID
} KernelType;

typedef struct
{
    uint32_t pid;
    char name[256];
    uint32_t priority;
    uint64_t memory_usage;
    bool is_running;
} Process;

typedef struct
{
    KernelType type;
    uint32_t version_major;
    uint32_t version_minor;
    uint32_t version_patch;
    Process *processes;
    uint32_t process_count;
    uint64_t total_memory;
    uint64_t used_memory;
    uint32_t cpu_cores;
    float cpu_usage;
} KernelCore;

KernelCore *kernel_init(KernelType type);
void kernel_cleanup(KernelCore *kernel);
void kernel_start_process(KernelCore *kernel, const char *name, uint32_t priority);
void kernel_stop_process(KernelCore *kernel, uint32_t pid);
void kernel_schedule_tasks(KernelCore *kernel);
uint64_t kernel_get_memory_available(KernelCore *kernel);
float kernel_get_cpu_usage(KernelCore *kernel);
int kernel_main_loop(KernelCore *kernel);

#endif
