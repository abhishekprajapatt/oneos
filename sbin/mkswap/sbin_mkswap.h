#ifndef SBIN_MKSWAP_H
#define SBIN_MKSWAP_H

#include <stdint.h>

typedef struct
{
    uint32_t swap_id;
    const char *device;
    uint64_t swap_size;
    uint32_t swap_status;
} sbin_mkswap_entry_t;

typedef struct
{
    uint32_t total_swap_ops;
    uint32_t successful_swap;
    uint32_t failed_swap;
} sbin_mkswap_state_t;

int sbin_mkswap_init(void);
int sbin_mkswap_create(sbin_mkswap_entry_t *entry);
int sbin_mkswap_verify(const char *device);
int sbin_mkswap_get_status(uint32_t swap_id, sbin_mkswap_entry_t *entry);
int sbin_mkswap_get_state(sbin_mkswap_state_t *state);

#endif
