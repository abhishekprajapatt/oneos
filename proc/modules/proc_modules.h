#ifndef PROC_MODULES_H
#define PROC_MODULES_H

#include <stdint.h>

typedef struct
{
    uint32_t module_id;
    const char *module_name;
    uint64_t module_size;
    uint32_t use_count;
    char *used_by;
} proc_modules_entry_t;

typedef struct
{
    uint32_t total_modules;
    uint32_t loaded_modules;
    uint64_t total_module_memory;
} proc_modules_state_t;

int proc_modules_init(void);
int proc_modules_list(proc_modules_entry_t *modules, uint32_t *count);
int proc_modules_get_module(const char *module_name, proc_modules_entry_t *module);
int proc_modules_get_dependencies(const char *module_name, char **deps, uint32_t *count);
int proc_modules_get_references(uint32_t module_id, uint32_t *ref_count);
int proc_modules_get_state(proc_modules_state_t *state);

#endif
