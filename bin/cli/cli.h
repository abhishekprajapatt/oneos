#ifndef CLI_H
#define CLI_H

#include <stdint.h>

typedef struct
{
    uint32_t command_id;
    const char *command_name;
    const char *description;
} cli_config_t;

int cli_init(void);
int cli_parse_arguments(int argc, const char **argv);
int cli_execute_command(const char *command);
int cli_display_help(void);
int cli_display_version(void);
int cli_process_input(const char *input_buffer);

#endif
