#ifndef SBIN_UPDATEINITRAMFS_H
#define SBIN_UPDATEINITRAMFS_H

#include <stdint.h>

typedef struct
{
    uint32_t update_id;
    const char *kernel_version;
    uint32_t update_status;
    uint64_t initramfs_size;
} sbin_updateinitramfs_entry_t;

typedef struct
{
    uint32_t total_updates;
    uint32_t successful_updates;
    uint32_t failed_updates;
} sbin_updateinitramfs_state_t;

int sbin_updateinitramfs_init(void);
int sbin_updateinitramfs_update(sbin_updateinitramfs_entry_t *entry);
int sbin_updateinitramfs_verify(const char *kernel_version);
int sbin_updateinitramfs_get_status(uint32_t update_id, sbin_updateinitramfs_entry_t *entry);
int sbin_updateinitramfs_get_state(sbin_updateinitramfs_state_t *state);

#endif
