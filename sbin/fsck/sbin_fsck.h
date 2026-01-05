#ifndef SBIN_FSCK_H
#define SBIN_FSCK_H

#include <stdint.h>

typedef struct
{
    uint32_t check_id;
    const char *device;
    const char *fs_type;
    uint32_t check_status;
    uint32_t errors_found;
} sbin_fsck_entry_t;

typedef struct
{
    uint32_t total_checks;
    uint32_t passed_checks;
    uint32_t failed_checks;
    uint32_t repaired_errors;
} sbin_fsck_state_t;

int sbin_fsck_init(void);
int sbin_fsck_check_filesystem(sbin_fsck_entry_t *entry);
int sbin_fsck_repair(const char *device, uint32_t flags);
int sbin_fsck_get_result(uint32_t check_id, sbin_fsck_entry_t *entry);
int sbin_fsck_get_state(sbin_fsck_state_t *state);

#endif
