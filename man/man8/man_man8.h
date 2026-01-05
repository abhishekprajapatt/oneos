#ifndef MAN_MAN8_H
#define MAN_MAN8_H

#include <stdint.h>

typedef struct
{
    uint32_t admin_cmd_id;
    const char *command_name;
    const char *synopsis;
    const char *description;
    uint32_t privilege_level;
} man_man8_entry_t;

typedef struct
{
    uint32_t total_admin_commands;
    uint32_t documented_admin_commands;
    uint32_t privilege_levels;
} man_man8_state_t;

int man_man8_init(void);
int man_man8_add_command(man_man8_entry_t *entry);
int man_man8_remove_command(uint32_t admin_cmd_id);
int man_man8_get_command(const char *name, man_man8_entry_t *entry);
int man_man8_get_privileged_commands(uint32_t min_level, man_man8_entry_t *results, uint32_t *count);

#endif
