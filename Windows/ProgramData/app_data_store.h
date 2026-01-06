#ifndef APP_DATA_STORE_H
#define APP_DATA_STORE_H

#include <stdint.h>

typedef struct
{
    const char *key;
    const char *value;
    uint32_t data_type;
} app_data_entry_t;

int app_data_store_init(void);
int app_data_store_set(const char *app_name, app_data_entry_t *entry);
int app_data_store_get(const char *app_name, const char *key, char *value, uint32_t max_len);
int app_data_store_delete(const char *app_name, const char *key);
int app_data_store_list_keys(const char *app_name, char **keys, uint32_t max_keys);
int app_data_store_backup(const char *app_name, const char *backup_path);
int app_data_store_restore(const char *app_name, const char *backup_path);

#endif
