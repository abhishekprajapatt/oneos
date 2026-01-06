#ifndef COMMON_FILES_H
#define COMMON_FILES_H

#include <stdint.h>

int common_files_init(void);
int common_files_register(const char *file_name, const char *file_path);
int common_files_unregister(const char *file_name);
int common_files_list(char **files, uint32_t max_files);
int common_files_get_path(const char *file_name, char *path, uint32_t max_len);
int common_files_verify(const char *file_name);

#endif
