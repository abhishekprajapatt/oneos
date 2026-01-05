#ifndef SBIN_DEPMOD_H
#define SBIN_DEPMOD_H

#include <stdint.h>

typedef struct
{
    uint32_t module_id;
    const char *module_name;
    const char *module_path;
    uint32_t dependency_count;
} sbin_depmod_entry_t;

typedef struct
{
    uint32_t total_modules;
    uint32_t modules_analyzed;
    uint32_t dependency_errors;
} sbin_depmod_state_t;

int sbin_depmod_init(void);
int sbin_depmod_analyze(const char *kernel_version);
int sbin_depmod_get_dependencies(const char *module, char **deps, uint32_t *count);
int sbin_depmod_update_map(void);
int sbin_depmod_get_state(sbin_depmod_state_t *state);

#endif
