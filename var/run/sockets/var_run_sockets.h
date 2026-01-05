#ifndef VAR_RUN_SOCKETS_H
#define VAR_RUN_SOCKETS_H

#include <stdint.h>

typedef struct
{
    uint32_t socket_id;
    const char *socket_name;
    const char *socket_type;
    const char *socket_path;
    uint32_t is_active;
} var_run_sockets_entry_t;

typedef struct
{
    uint32_t total_sockets;
    uint32_t active_sockets;
    uint32_t inactive_sockets;
} var_run_sockets_state_t;

int var_run_sockets_init(void);
int var_run_sockets_list(var_run_sockets_entry_t *sockets, uint32_t *count);
int var_run_sockets_get_socket(const char *socket_name, var_run_sockets_entry_t *socket);
int var_run_sockets_register(const char *socket_name, const char *socket_type, const char *socket_path);
int var_run_sockets_unregister(const char *socket_name);
int var_run_sockets_check_status(const char *socket_name, uint32_t *active);
int var_run_sockets_get_state(var_run_sockets_state_t *state);

#endif
