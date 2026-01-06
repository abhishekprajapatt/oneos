#ifndef SYSTEM32_CONFIG_H
#define SYSTEM32_CONFIG_H

#include <stdint.h>

typedef struct
{
    const char *config_name;
    const char *config_value;
    uint32_t config_type;
} system_config_t;

int system32_config_init(void);
int system32_config_load(const char *config_file);
int system32_config_set(system_config_t *config);
int system32_config_get(const char *config_name, char *value, uint32_t max_len);
int system32_config_list(system_config_t **configs, uint32_t *count);
int system32_config_save(const char *config_file);

#endif
