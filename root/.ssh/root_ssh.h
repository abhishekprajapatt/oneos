#ifndef ROOT_SSH_H
#define ROOT_SSH_H

#include <stdint.h>

typedef struct
{
    uint32_t key_id;
    const char *key_name;
    const char *public_key;
    const char *private_key_path;
    uint32_t key_status;
} root_ssh_key_t;

typedef struct
{
    uint32_t total_keys;
    uint32_t active_keys;
    uint32_t revoked_keys;
    uint32_t key_permissions;
} root_ssh_state_t;

int root_ssh_init(void);
int root_ssh_generate_key(root_ssh_key_t *key);
int root_ssh_add_key(root_ssh_key_t *key);
int root_ssh_remove_key(uint32_t key_id);
int root_ssh_get_key(uint32_t key_id, root_ssh_key_t *key);
int root_ssh_set_permissions(const char *key_path, uint32_t permissions);
int root_ssh_get_state(root_ssh_state_t *state);

#endif
