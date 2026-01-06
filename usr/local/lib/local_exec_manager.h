#ifndef LOCAL_EXEC_MANAGER_H
#define LOCAL_EXEC_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *executable_name;
    const char *executable_path;
    const char *version;
    uint32_t permissions;
} local_exec_info_t;

int local_exec_manager_init(void);
int local_exec_manager_register_executable(local_exec_info_t *exec_info);
int local_exec_manager_unregister_executable(const char *executable_name);
int local_exec_manager_find_executable(const char *executable_name, char *path, uint32_t max_len);
int local_exec_manager_list_executables(char **executables, uint32_t max_executables);
int local_exec_manager_execute(const char *executable_name, const char **args, uint32_t arg_count);

#endif
