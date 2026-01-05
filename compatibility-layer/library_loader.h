#ifndef LIBRARY_LOADER_H
#define LIBRARY_LOADER_H

#include <stdint.h>

typedef struct
{
    uint32_t lib_id;
    const char *lib_name;
    const char *version;
    void *handle;
} library_info_t;

int loader_init(void);
int loader_load_library(const char *lib_name, library_info_t *info);
int loader_unload_library(uint32_t lib_id);
int loader_resolve_symbol(uint32_t lib_id, const char *symbol, void **symbol_ptr);
int loader_get_library_version(const char *lib_name, char *version, uint32_t max_len);
int loader_validate_abi(uint32_t lib_id);

#endif
