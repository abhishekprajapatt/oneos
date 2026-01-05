#ifndef MAN_MAN1_H
#define MAN_MAN1_H

#include <stdint.h>

typedef struct
{
    uint32_t cmd_id;
    const char *command_name;
    const char *synopsis;
    const char *description;
    uint32_t category;
} man_man1_entry_t;

typedef struct
{
    uint32_t total_commands;
    uint32_t documented_commands;
    uint32_t categories_count;
} man_man1_state_t;

int man_man1_init(void);
int man_man1_add_command(man_man1_entry_t *entry);
int man_man1_remove_command(uint32_t cmd_id);
int man_man1_get_command(const char *name, man_man1_entry_t *entry);
int man_man1_search(const char *keyword, man_man1_entry_t *results, uint32_t *count);

#endif
