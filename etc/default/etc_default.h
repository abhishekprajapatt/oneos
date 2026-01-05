#ifndef ETC_DEFAULT_H
#define ETC_DEFAULT_H

#include <stdint.h>

typedef struct
{
    uint32_t config_id;
    const char *param_name;
    const char *param_value;
    uint32_t flags;
} etc_default_config_t;

typedef struct
{
    uint32_t entry_count;
    uint32_t max_entries;
    uint32_t version;
} etc_default_manager_t;

int etc_default_init(void);
int etc_default_load_config(const char *filename, etc_default_config_t *config);
int etc_default_save_config(const char *filename, etc_default_config_t *config);
int etc_default_get_param(const char *filename, const char *param_name, char *value, uint32_t size);
int etc_default_set_param(const char *filename, const char *param_name, const char *param_value);
int etc_default_delete_param(const char *filename, const char *param_name);

#endif
