#ifndef TERMINAL_H
#define TERMINAL_H

#include <stdint.h>

typedef struct
{
    uint32_t terminal_id;
    const char *current_directory;
    const char *shell_name;
    uint32_t is_active;
} terminal_state_t;

int terminal_init(void);
int terminal_create_instance(uint32_t *terminal_id);
int terminal_close_instance(uint32_t terminal_id);
int terminal_execute_command(uint32_t terminal_id, const char *command);
int terminal_get_output(uint32_t terminal_id, char *output, uint32_t max_len);
int terminal_clear_screen(uint32_t terminal_id);
int terminal_change_directory(uint32_t terminal_id, const char *path);
int terminal_list_instances(terminal_state_t *instances, uint32_t *count);

#endif
