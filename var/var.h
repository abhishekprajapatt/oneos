#ifndef VAR_H
#define VAR_H

#include <stdint.h>

int var_init(void);
int var_create_directory(const char *path);
int var_write_log(const char *component, const char *message);
int var_get_log(const char *component, char *buffer, uint32_t max_len);
int var_rotate_logs(const char *component);

#endif
