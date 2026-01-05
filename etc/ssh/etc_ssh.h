#ifndef ETC_SSH_H
#define ETC_SSH_H

#include <stdint.h>

typedef struct
{
    uint32_t config_id;
    const char *key;
    const char *value;
    uint32_t flags;
} etc_ssh_config_t;

typedef struct
{
    uint32_t port;
    uint32_t permit_root_login;
    uint32_t password_auth;
    uint32_t pubkey_auth;
} etc_ssh_server_config_t;

int etc_ssh_init(void);
int etc_ssh_load_config(const char *filename);
int etc_ssh_set_config(etc_ssh_config_t *config);
int etc_ssh_get_server_config(etc_ssh_server_config_t *config);
int etc_ssh_set_server_config(etc_ssh_server_config_t *config);
int etc_ssh_save_config(const char *filename);

#endif
