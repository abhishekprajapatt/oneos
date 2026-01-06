#ifndef ENVIRONMENT_VARS_H
#define ENVIRONMENT_VARS_H

#include <stdint.h>

int environment_vars_init(void);
int environment_vars_set(const char *var_name, const char *value);
int environment_vars_get(const char *var_name, char *value, uint32_t max_len);
int environment_vars_delete(const char *var_name);
int environment_vars_list(char **vars, uint32_t max_vars);
int environment_vars_expand(const char *input, char *output, uint32_t max_len);

#endif
