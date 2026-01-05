#ifndef SBIN_HALT_H
#define SBIN_HALT_H

#include <stdint.h>

typedef struct
{
    uint32_t halt_id;
    uint32_t halt_type;
    uint32_t halt_status;
    uint64_t halt_time;
} sbin_halt_entry_t;

typedef struct
{
    uint32_t total_halts;
    uint32_t successful_halts;
    uint32_t failed_halts;
} sbin_halt_state_t;

int sbin_halt_init(void);
int sbin_halt_system(sbin_halt_entry_t *entry);
int sbin_halt_cancel(void);
int sbin_halt_get_status(uint32_t halt_id, sbin_halt_entry_t *entry);
int sbin_halt_get_state(sbin_halt_state_t *state);

#endif
