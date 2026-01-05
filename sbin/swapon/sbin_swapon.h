#ifndef SBIN_SWAPON_H
#define SBIN_SWAPON_H

#include <stdint.h>

typedef struct
{
    uint32_t swapon_id;
    const char *swap_device;
    uint32_t swapon_status;
} sbin_swapon_entry_t;

typedef struct
{
    uint32_t total_swapon_ops;
    uint32_t successful_swapon;
    uint32_t failed_swapon;
} sbin_swapon_state_t;

int sbin_swapon_init(void);
int sbin_swapon_enable(sbin_swapon_entry_t *entry);
int sbin_swapon_verify(const char *swap_device);
int sbin_swapon_get_status(uint32_t swapon_id, sbin_swapon_entry_t *entry);
int sbin_swapon_get_state(sbin_swapon_state_t *state);

#endif
