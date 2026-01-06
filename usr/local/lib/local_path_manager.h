#ifndef LOCAL_PATH_MANAGER_H
#define LOCAL_PATH_MANAGER_H

#include <stdint.h>

int local_path_manager_init(void);
int local_path_manager_add_to_path(const char *directory);
int local_path_manager_remove_from_path(const char *directory);
int local_path_manager_get_path(char *path, uint32_t max_len);
int local_path_manager_list_path_entries(char **entries, uint32_t max_entries);
int local_path_manager_search_executable(const char *executable_name, char *path, uint32_t max_len);
int local_path_manager_verify_path(void);

#endif
