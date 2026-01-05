#ifndef SBIN_GRUBINSTALL_H
#define SBIN_GRUBINSTALL_H

#include <stdint.h>

typedef struct
{
    uint32_t install_id;
    const char *boot_device;
    const char *target;
    uint32_t install_status;
} sbin_grubinstall_entry_t;

typedef struct
{
    uint32_t total_installs;
    uint32_t successful_installs;
    uint32_t failed_installs;
} sbin_grubinstall_state_t;

int sbin_grubinstall_init(void);
int sbin_grubinstall_install(sbin_grubinstall_entry_t *entry);
int sbin_grubinstall_check(const char *boot_device);
int sbin_grubinstall_get_status(uint32_t install_id, sbin_grubinstall_entry_t *entry);
int sbin_grubinstall_get_state(sbin_grubinstall_state_t *state);

#endif
