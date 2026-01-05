#ifndef SBIN_REBOOT_H
#define SBIN_REBOOT_H

#include <stdint.h>

typedef struct
{
    uint32_t reboot_id;
    uint32_t reboot_type;
    uint32_t reboot_status;
    uint64_t reboot_time;
} sbin_reboot_entry_t;

typedef struct
{
    uint32_t total_reboots;
    uint32_t successful_reboots;
    uint32_t failed_reboots;
} sbin_reboot_state_t;

int sbin_reboot_init(void);
int sbin_reboot_system(sbin_reboot_entry_t *entry);
int sbin_reboot_cancel(void);
int sbin_reboot_get_status(uint32_t reboot_id, sbin_reboot_entry_t *entry);
int sbin_reboot_get_state(sbin_reboot_state_t *state);

#endif
