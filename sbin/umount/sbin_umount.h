#ifndef SBIN_UMOUNT_H
#define SBIN_UMOUNT_H

#include <stdint.h>

typedef struct
{
    uint32_t umount_id;
    const char *target;
    uint32_t umount_status;
} sbin_umount_entry_t;

typedef struct
{
    uint32_t total_umount_ops;
    uint32_t successful_umounts;
    uint32_t failed_umounts;
} sbin_umount_state_t;

int sbin_umount_init(void);
int sbin_umount_unmount(sbin_umount_entry_t *entry);
int sbin_umount_is_mounted(const char *target);
int sbin_umount_get_status(uint32_t umount_id, sbin_umount_entry_t *entry);
int sbin_umount_get_state(sbin_umount_state_t *state);

#endif
