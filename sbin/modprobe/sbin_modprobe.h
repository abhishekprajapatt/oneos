#ifndef SBIN_MODPROBE_H
#define SBIN_MODPROBE_H

#include <stdint.h>

typedef struct
{
    uint32_t modprobe_id;
    const char *module_name;
    const char *module_args;
    uint32_t modprobe_status;
} sbin_modprobe_entry_t;

typedef struct
{
    uint32_t total_modprobe_ops;
    uint32_t successful_loads;
    uint32_t failed_loads;
} sbin_modprobe_state_t;

int sbin_modprobe_init(void);
int sbin_modprobe_load(sbin_modprobe_entry_t *entry);
int sbin_modprobe_unload(const char *module_name);
int sbin_modprobe_get_status(uint32_t modprobe_id, sbin_modprobe_entry_t *entry);
int sbin_modprobe_get_state(sbin_modprobe_state_t *state);

#endif
