#ifndef SBIN_INSMOD_H
#define SBIN_INSMOD_H

#include <stdint.h>

typedef struct
{
    uint32_t module_id;
    const char *module_path;
    const char *module_args;
    uint32_t insert_status;
} sbin_insmod_entry_t;

typedef struct
{
    uint32_t total_inserts;
    uint32_t successful_inserts;
    uint32_t failed_inserts;
} sbin_insmod_state_t;

int sbin_insmod_init(void);
int sbin_insmod_insert(sbin_insmod_entry_t *entry);
int sbin_insmod_check_module(const char *module_path);
int sbin_insmod_get_status(uint32_t module_id, sbin_insmod_entry_t *entry);
int sbin_insmod_get_state(sbin_insmod_state_t *state);

#endif
