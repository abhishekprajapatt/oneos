#ifndef SHORTCUT_MANAGER_H
#define SHORTCUT_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *shortcut_name;
    const char *target_path;
    const char *working_dir;
    const char *arguments;
} shortcut_t;

int shortcut_manager_init(void);
int shortcut_manager_create(shortcut_t *shortcut);
int shortcut_manager_delete(const char *shortcut_name);
int shortcut_manager_get(const char *shortcut_name, shortcut_t *shortcut);
int shortcut_manager_list(char **shortcuts, uint32_t max_shortcuts);
int shortcut_manager_resolve(const char *shortcut_path, char *target, uint32_t max_len);

#endif
