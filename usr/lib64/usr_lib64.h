#ifndef USR_LIB64_H
#define USR_LIB64_H

#include <stdint.h>

typedef struct
{
    uint32_t lib64_id;
    const char *library_name;
    const char *library_version;
    const char *library_path;
    uint64_t size;
} usr_lib64_entry_t;

typedef struct
{
    uint32_t total_libraries;
    uint32_t static_libraries;
    uint32_t dynamic_libraries;
    uint64_t total_size;
} usr_lib64_state_t;

int usr_lib64_init(void);
int usr_lib64_list_libraries(usr_lib64_entry_t *libraries, uint32_t *count);
int usr_lib64_get_library(const char *name, usr_lib64_entry_t *library);
int usr_lib64_load_library(const char *library_name, void **handle);
int usr_lib64_unload_library(void *handle);
int usr_lib64_get_state(usr_lib64_state_t *state);

#endif
