#ifndef SBIN_RMMOD_H
#define SBIN_RMMOD_H

#include <stdint.h>

typedef struct
{
    uint32_t rmmod_id;
    const char *module_name;
    uint32_t rmmod_status;
} sbin_rmmod_entry_t;

typedef struct
{
    uint32_t total_rmmod_ops;
    uint32_t successful_removes;
    uint32_t failed_removes;
} sbin_rmmod_state_t;

int sbin_rmmod_init(void);
int sbin_rmmod_remove(sbin_rmmod_entry_t *entry);
int sbin_rmmod_check_dependencies(const char *module_name);
int sbin_rmmod_get_status(uint32_t rmmod_id, sbin_rmmod_entry_t *entry);
int sbin_rmmod_get_state(sbin_rmmod_state_t *state);

#endif
