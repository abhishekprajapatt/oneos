#ifndef PROCESS_MANAGER_H
#define PROCESS_MANAGER_H

#include <stdint.h>

typedef struct
{
    uint64_t pid;
    const char *process_name;
    uint32_t priority;
    uint32_t status;
    uint64_t memory_usage;
} process_info_t;

int process_manager_init(void);
int process_manager_create_process(const char *path, const char **args);
int process_manager_terminate_process(uint64_t pid);
int process_manager_suspend_process(uint64_t pid);
int process_manager_resume_process(uint64_t pid);
int process_manager_set_priority(uint64_t pid, uint32_t priority);
int process_manager_get_info(uint64_t pid, process_info_t *info);

#endif
