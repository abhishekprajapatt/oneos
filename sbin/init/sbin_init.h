#ifndef SBIN_INIT_H
#define SBIN_INIT_H

#include <stdint.h>

typedef struct
{
    uint32_t init_pid;
    uint32_t runlevel;
    uint32_t init_status;
    uint64_t uptime;
} sbin_init_entry_t;

typedef struct
{
    uint32_t total_processes;
    uint32_t running_processes;
    uint32_t stopped_processes;
} sbin_init_state_t;

int sbin_init_init(void);
int sbin_init_set_runlevel(uint32_t runlevel);
int sbin_init_get_runlevel(uint32_t *runlevel);
int sbin_init_spawn_service(const char *service);
int sbin_init_get_state(sbin_init_state_t *state);

#endif
