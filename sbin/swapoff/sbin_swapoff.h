#ifndef SBIN_SWAPOFF_H
#define SBIN_SWAPOFF_H

#include <stdint.h>

typedef struct
{
    uint32_t swapoff_id;
    const char *swap_device;
    uint32_t swapoff_status;
} sbin_swapoff_entry_t;

typedef struct
{
    uint32_t total_swapoff_ops;
    uint32_t successful_swapoffs;
    uint32_t failed_swapoffs;
} sbin_swapoff_state_t;

int sbin_swapoff_init(void);
int sbin_swapoff_disable(sbin_swapoff_entry_t *entry);
int sbin_swapoff_verify(const char *swap_device);
int sbin_swapoff_get_status(uint32_t swapoff_id, sbin_swapoff_entry_t *entry);
int sbin_swapoff_get_state(sbin_swapoff_state_t *state);

#endif
