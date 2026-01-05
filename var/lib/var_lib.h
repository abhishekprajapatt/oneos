#ifndef VAR_LIB_H
#define VAR_LIB_H

#include <stdint.h>

typedef struct
{
    uint32_t lib_id;
    const char *lib_name;
    const char *lib_type;
    const char *lib_path;
    uint64_t lib_size;
} var_lib_entry_t;

typedef struct
{
    uint32_t total_libraries;
    uint32_t active_libraries;
    uint64_t total_lib_size;
} var_lib_state_t;

int var_lib_init(void);
int var_lib_list(var_lib_entry_t *libraries, uint32_t *count);
int var_lib_get(const char *lib_name, var_lib_entry_t *library);
int var_lib_register(const char *name, const char *type, const char *path);
int var_lib_unregister(const char *lib_name);
int var_lib_get_state(var_lib_state_t *state);

#endif
