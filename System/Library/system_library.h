#ifndef SYSTEM_LIBRARY_H
#define SYSTEM_LIBRARY_H

#include <stdint.h>

typedef struct
{
    uint32_t lib_id;
    const char *lib_name;
    const char *lib_path;
    const char *version;
    uint32_t lib_status;
} system_library_entry_t;

typedef struct
{
    uint32_t total_libraries;
    uint32_t loaded_libraries;
    uint32_t unloaded_libraries;
    uint64_t total_lib_size;
} system_library_state_t;

int system_library_init(void);
int system_library_load(system_library_entry_t *library);
int system_library_unload(uint32_t lib_id);
int system_library_get_library(uint32_t lib_id, system_library_entry_t *library);
int system_library_get_version(const char *lib_name, char *version);
int system_library_get_state(system_library_state_t *state);

#endif
