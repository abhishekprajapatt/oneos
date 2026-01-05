#include "security_keys.h"

int security_keys_init(void)
{
    return 0;
}

int security_keys_generate(security_keys_entry_t *key)
{
    if (!key)
    {
        return -1;
    }
    return 0;
}

int security_keys_revoke(uint32_t key_id)
{
    return 0;
}

int security_keys_get_key(uint32_t key_id, security_keys_entry_t *key)
{
    if (!key)
    {
        return -1;
    }
    return 0;
}

int security_keys_verify_signature(const void *data, uint64_t data_size, uint32_t key_id)
{
    if (!data)
    {
        return -1;
    }
    return 0;
}

int security_keys_get_state(security_keys_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
