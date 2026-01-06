#ifndef UNINSTALLER_H
#define UNINSTALLER_H

#include <stdint.h>

typedef struct
{
    const char *program_name;
    const char *uninstall_path;
    uint32_t files_count;
    uint32_t registry_entries;
} uninstall_info_t;

int uninstaller_init(void);
int uninstaller_register(uninstall_info_t *info);
int uninstaller_execute(const char *program_name);
int uninstaller_clean_registry(const char *program_name);
int uninstaller_remove_files(const char **file_paths, uint32_t count);
int uninstaller_rollback(const char *program_name);

#endif
