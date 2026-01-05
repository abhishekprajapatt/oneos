#ifndef SBIN_GRUBMKCONFIG_H
#define SBIN_GRUBMKCONFIG_H

#include <stdint.h>

typedef struct
{
    uint32_t config_id;
    const char *output_path;
    const char *search_path;
    uint32_t config_status;
} sbin_grubmkconfig_entry_t;

typedef struct
{
    uint32_t total_configs;
    uint32_t generated_configs;
    uint32_t config_errors;
} sbin_grubmkconfig_state_t;

int sbin_grubmkconfig_init(void);
int sbin_grubmkconfig_generate(sbin_grubmkconfig_entry_t *entry);
int sbin_grubmkconfig_validate(const char *config_path);
int sbin_grubmkconfig_get_status(uint32_t config_id, sbin_grubmkconfig_entry_t *entry);
int sbin_grubmkconfig_get_state(sbin_grubmkconfig_state_t *state);

#endif
