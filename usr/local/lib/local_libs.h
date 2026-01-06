#ifndef LOCAL_LIBS_H
#define LOCAL_LIBS_H

#include <stdint.h>

typedef struct
{
    const char *library_name;
    const char *library_path;
    const char *version;
    uint32_t type;
} local_lib_info_t;

int local_libs_init(void);
int local_libs_register_library(local_lib_info_t *lib_info);
int local_libs_unregister_library(const char *library_name);
int local_libs_find_library(const char *library_name, char *path, uint32_t max_len);
int local_libs_list_libraries(local_lib_info_t **libraries, uint32_t *count);
int local_libs_install_library(const char *library_file);
int local_libs_link_library(const char *library_name);

#endif
