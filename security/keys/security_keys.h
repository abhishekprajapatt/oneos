#ifndef SECURITY_KEYS_H
#define SECURITY_KEYS_H

#include <stdint.h>

typedef struct
{
    uint32_t key_id;
    const char *key_name;
    const char *key_type;
    uint64_t key_size;
    uint32_t key_status;
} security_keys_entry_t;

typedef struct
{
    uint32_t total_keys;
    uint32_t active_keys;
    uint32_t revoked_keys;
    uint32_t expired_keys;
} security_keys_state_t;

int security_keys_init(void);
int security_keys_generate(security_keys_entry_t *key);
int security_keys_revoke(uint32_t key_id);
int security_keys_get_key(uint32_t key_id, security_keys_entry_t *key);
int security_keys_verify_signature(const void *data, uint64_t data_size, uint32_t key_id);
int security_keys_get_state(security_keys_state_t *state);

#endif
