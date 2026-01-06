#ifndef BIN_H
#define BIN_H

#include <stdint.h>

int bin_init(void);
int bin_exec(const char *program, const char **args);
int bin_spawn_process(const char *program, const char **args, uint32_t *pid);
int bin_wait_process(uint32_t pid);
int bin_kill_process(uint32_t pid, uint32_t signal);

#endif
