#ifndef SBIN_POWEROFF_H
#define SBIN_POWEROFF_H

#include <stdint.h>

typedef struct
{
    uint32_t poweroff_id;
    uint32_t poweroff_type;
    uint32_t poweroff_status;
    uint64_t poweroff_time;
} sbin_poweroff_entry_t;

typedef struct
{
    uint32_t total_poweroffs;
    uint32_t successful_poweroffs;
    uint32_t failed_poweroffs;
} sbin_poweroff_state_t;

int sbin_poweroff_init(void);
int sbin_poweroff_system(sbin_poweroff_entry_t *entry);
int sbin_poweroff_cancel(void);
int sbin_poweroff_get_status(uint32_t poweroff_id, sbin_poweroff_entry_t *entry);
int sbin_poweroff_get_state(sbin_poweroff_state_t *state);

#endif
