#ifndef SBIN_SHUTDOWN_H
#define SBIN_SHUTDOWN_H

#include <stdint.h>

typedef struct
{
    uint32_t shutdown_id;
    uint32_t shutdown_type;
    uint32_t shutdown_delay;
    uint32_t shutdown_status;
} sbin_shutdown_entry_t;

typedef struct
{
    uint32_t total_shutdowns;
    uint32_t successful_shutdowns;
    uint32_t failed_shutdowns;
} sbin_shutdown_state_t;

int sbin_shutdown_init(void);
int sbin_shutdown_schedule(sbin_shutdown_entry_t *entry);
int sbin_shutdown_cancel(void);
int sbin_shutdown_get_status(uint32_t shutdown_id, sbin_shutdown_entry_t *entry);
int sbin_shutdown_get_state(sbin_shutdown_state_t *state);

#endif
