#ifndef TOOLS_H
#define TOOLS_H

#include <stdint.h>

int tools_init(void);
int tools_run_command(const char *cmd, const char **args);
int tools_parse_config(const char *config_file);
int tools_get_version(char *version, uint32_t max_len);

#endif
