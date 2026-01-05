#ifndef INIT_BOOT_H
#define INIT_BOOT_H

#include <stdint.h>

typedef struct
{
    uint32_t boot_id;
    const char *boot_name;
    uint32_t boot_order;
    uint32_t timeout;
} init_boot_sequence_t;

typedef struct
{
    uint32_t sequence_count;
    uint32_t completed_count;
    uint32_t boot_status;
} init_boot_state_t;

int init_boot_load_config(const char *config_file);
int init_boot_add_sequence(init_boot_sequence_t *sequence);
int init_boot_remove_sequence(uint32_t boot_id);
int init_boot_execute_sequence(uint32_t boot_id);
int init_boot_get_status(uint32_t *status);
int init_boot_wait_complete(uint32_t timeout);

#endif
