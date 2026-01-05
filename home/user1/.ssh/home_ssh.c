#include "home_ssh.h"

int home_ssh_init(const char *username)
{
    if (!username)
    {
        return -1;
    }
    return 0;
}

int home_ssh_load_keys(const char *ssh_dir)
{
    if (!ssh_dir)
    {
        return -1;
    }
    return 0;
}

int home_ssh_add_public_key(home_ssh_pubkey_t *pubkey)
{
    if (!pubkey)
    {
        return -1;
    }
    return 0;
}

int home_ssh_remove_public_key(uint32_t key_id)
{
    return 0;
}

int home_ssh_verify_key(const char *key_name)
{
    if (!key_name)
    {
        return -1;
    }
    return 0;
}

int home_ssh_get_key_fingerprint(uint32_t key_id, char *fingerprint, uint32_t size)
{
    if (!fingerprint)
    {
        return -1;
    }
    return 0;
}
