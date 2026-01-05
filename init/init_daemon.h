#ifndef INIT_DAEMON_H
#define INIT_DAEMON_H

#include <stdint.h>

typedef struct
{
    uint32_t daemon_id;
    const char *daemon_name;
    const char *daemon_path;
    uint32_t flags;
} init_daemon_config_t;

typedef struct
{
    uint32_t daemon_count;
    uint32_t active_daemons;
    uint32_t failed_daemons;
} init_daemon_state_t;

int init_daemon_spawn(init_daemon_config_t *config);
int init_daemon_kill(uint32_t daemon_id);
int init_daemon_respawn(uint32_t daemon_id);
int init_daemon_wait(uint32_t daemon_id, uint32_t timeout);
int init_daemon_signal(uint32_t daemon_id, uint32_t signal);
int init_daemon_get_info(uint32_t daemon_id, init_daemon_config_t *config);

#endif
