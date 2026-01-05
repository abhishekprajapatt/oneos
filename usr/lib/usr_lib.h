#ifndef USR_LIB_H
#define USR_LIB_H

#include <stdint.h>

typedef struct
{
    uint32_t lib_id;
    const char *library_name;
    const char *library_version;
    const char *library_path;
    uint64_t size;
} usr_lib_entry_t;

typedef struct
{
    uint32_t total_libraries;
    uint32_t static_libraries;
    uint32_t dynamic_libraries;
    uint64_t total_size;
} usr_lib_state_t;

int usr_lib_init(void);
int usr_lib_list_libraries(usr_lib_entry_t *libraries, uint32_t *count);
int usr_lib_get_library(const char *name, usr_lib_entry_t *library);
int usr_lib_load_library(const char *library_name, void **handle);
int usr_lib_unload_library(void *handle);
int usr_lib_get_state(usr_lib_state_t *state);

#endif
