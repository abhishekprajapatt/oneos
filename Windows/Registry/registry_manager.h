#ifndef REGISTRY_MANAGER_H
#define REGISTRY_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *hive_name;
    const char *hive_path;
    uint32_t flags;
} registry_hive_t;

int registry_manager_init(void);
int registry_manager_load_hive(registry_hive_t *hive);
int registry_manager_unload_hive(const char *hive_name);
int registry_manager_set_value(const char *hive_name, const char *key_path, const char *value_name, const char *value);
int registry_manager_get_value(const char *hive_name, const char *key_path, const char *value_name, char *value, uint32_t max_len);
int registry_manager_delete_key(const char *hive_name, const char *key_path);
int registry_manager_export_hive(const char *hive_name, const char *export_path);
int registry_manager_import_hive(const char *import_path);

#endif
