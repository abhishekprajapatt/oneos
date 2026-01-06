#ifndef SCRIPTS_H
#define SCRIPTS_H

#include <stdint.h>

int scripts_init(void);
int scripts_execute(const char *script_path, const char **args);
int scripts_register_hook(const char *hook_name, const char *script);
int scripts_run_hook(const char *hook_name);
int scripts_validate_syntax(const char *script_path);

#endif
