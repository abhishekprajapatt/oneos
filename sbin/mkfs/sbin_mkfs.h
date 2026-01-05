#ifndef SBIN_MKFS_H
#define SBIN_MKFS_H

#include <stdint.h>

typedef struct
{
    uint32_t mkfs_id;
    const char *device;
    const char *fs_type;
    uint64_t block_size;
    uint32_t mkfs_status;
} sbin_mkfs_entry_t;

typedef struct
{
    uint32_t total_mkfs_ops;
    uint32_t successful_mkfs;
    uint32_t failed_mkfs;
} sbin_mkfs_state_t;

int sbin_mkfs_init(void);
int sbin_mkfs_create(sbin_mkfs_entry_t *entry);
int sbin_mkfs_verify(const char *device);
int sbin_mkfs_get_status(uint32_t mkfs_id, sbin_mkfs_entry_t *entry);
int sbin_mkfs_get_state(sbin_mkfs_state_t *state);

#endif
