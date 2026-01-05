#ifndef SBIN_GRUBMKRESCUE_H
#define SBIN_GRUBMKRESCUE_H

#include <stdint.h>

typedef struct
{
    uint32_t rescue_id;
    const char *output_path;
    const char *platform;
    uint32_t rescue_status;
} sbin_grubmkrescue_entry_t;

typedef struct
{
    uint32_t total_rescues;
    uint32_t successful_rescues;
    uint32_t failed_rescues;
} sbin_grubmkrescue_state_t;

int sbin_grubmkrescue_init(void);
int sbin_grubmkrescue_create(sbin_grubmkrescue_entry_t *entry);
int sbin_grubmkrescue_validate(const char *iso_path);
int sbin_grubmkrescue_get_status(uint32_t rescue_id, sbin_grubmkrescue_entry_t *entry);
int sbin_grubmkrescue_get_state(sbin_grubmkrescue_state_t *state);

#endif
