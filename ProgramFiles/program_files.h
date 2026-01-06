#ifndef PROGRAM_FILES_H
#define PROGRAM_FILES_H

#include <stdint.h>

int program_files_init(void);
int program_files_install(const char *program_name, const char *path);
int program_files_uninstall(const char *program_name);
int program_files_list(char **programs, uint32_t max_programs);
int program_files_get_path(const char *program_name, char *path, uint32_t max_len);

#endif
