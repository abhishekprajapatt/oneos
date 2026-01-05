#ifndef HOME_SSH_H
#define HOME_SSH_H

#include <stdint.h>

typedef struct
{
    uint32_t key_id;
    const char *key_name;
    uint8_t *public_key;
    uint32_t public_key_size;
} home_ssh_pubkey_t;

typedef struct
{
    uint32_t identity_count;
    uint32_t max_identities;
    uint32_t auth_methods;
} home_ssh_config_t;

int home_ssh_init(const char *username);
int home_ssh_load_keys(const char *ssh_dir);
int home_ssh_add_public_key(home_ssh_pubkey_t *pubkey);
int home_ssh_remove_public_key(uint32_t key_id);
int home_ssh_verify_key(const char *key_name);
int home_ssh_get_key_fingerprint(uint32_t key_id, char *fingerprint, uint32_t size);

#endif
