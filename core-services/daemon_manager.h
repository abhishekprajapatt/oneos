#ifndef DAEMON_MANAGER_H
#define DAEMON_MANAGER_H

#include <stdint.h>

typedef struct
{
    uint32_t daemon_id;
    const char *daemon_name;
    const char *executable_path;
    uint32_t status;
    uint64_t pid;
} daemon_info_t;

int daemon_manager_init(void);
int daemon_manager_start_daemon(const char *daemon_name);
int daemon_manager_stop_daemon(uint32_t daemon_id);
int daemon_manager_restart_daemon(uint32_t daemon_id);
int daemon_manager_get_status(uint32_t daemon_id, uint32_t *status);
int daemon_manager_list_daemons(daemon_info_t *daemons, uint32_t *count);
int daemon_manager_register_daemon(daemon_info_t *daemon);

#endif
