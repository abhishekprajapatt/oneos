#ifndef SYS_KERNEL_H
#define SYS_KERNEL_H

#include <stdint.h>

typedef struct
{
    uint32_t module_id;
    const char *module_name;
    const char *version;
    uint32_t load_status;
} sys_kernel_module_t;

typedef struct
{
    uint32_t loaded_modules;
    uint32_t unloaded_modules;
    uint32_t total_modules;
    uint64_t kernel_memory;
} sys_kernel_state_t;

int sys_kernel_init(void);
int sys_kernel_load_module(sys_kernel_module_t *module);
int sys_kernel_unload_module(uint32_t module_id);
int sys_kernel_get_module(uint32_t module_id, sys_kernel_module_t *module);
int sys_kernel_list_modules(sys_kernel_module_t *modules, uint32_t *count);
int sys_kernel_get_state(sys_kernel_state_t *state);

#endif
