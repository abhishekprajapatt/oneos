#ifndef SERVICES_SSH_H
#define SERVICES_SSH_H

#include <stdint.h>

typedef struct
{
    uint32_t session_id;
    const char *client_addr;
    uint32_t client_port;
    const char *username;
    uint32_t session_state;
} services_ssh_session_t;

typedef struct
{
    uint32_t total_sessions;
    uint32_t active_sessions;
    uint32_t closed_sessions;
    uint32_t failed_auth_attempts;
} services_ssh_state_t;

int services_ssh_init(void);
int services_ssh_accept_connection(services_ssh_session_t *session);
int services_ssh_close_session(uint32_t session_id);
int services_ssh_authenticate_user(uint32_t session_id, const char *username, const char *password);
int services_ssh_execute_command(uint32_t session_id, const char *command);
int services_ssh_get_session_info(uint32_t session_id, services_ssh_session_t *session);
int services_ssh_get_state(services_ssh_state_t *state);

#endif
