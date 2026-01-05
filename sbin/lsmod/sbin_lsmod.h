#ifndef SBIN_LSMOD_H
#define SBIN_LSMOD_H

#include <stdint.h>

typedef struct
{
    uint32_t module_id;
    const char *module_name;
    uint64_t module_size;
    uint32_t use_count;
    char *used_by;
} sbin_lsmod_entry_t;

typedef struct
{
    uint32_t total_modules;
    uint32_t loaded_modules;
    uint64_t total_memory;
} sbin_lsmod_state_t;

int sbin_lsmod_init(void);
int sbin_lsmod_list(sbin_lsmod_entry_t *modules, uint32_t *count);
int sbin_lsmod_get_module(const char *name, sbin_lsmod_entry_t *entry);
int sbin_lsmod_get_dependencies(const char *module, char **deps, uint32_t *count);
int sbin_lsmod_get_state(sbin_lsmod_state_t *state);

#endif
