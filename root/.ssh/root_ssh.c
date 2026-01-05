#include "root_ssh.h"

int root_ssh_init(void)
{
    return 0;
}

int root_ssh_generate_key(root_ssh_key_t *key)
{
    if (!key)
    {
        return -1;
    }
    return 0;
}

int root_ssh_add_key(root_ssh_key_t *key)
{
    if (!key)
    {
        return -1;
    }
    return 0;
}

int root_ssh_remove_key(uint32_t key_id)
{
    return 0;
}

int root_ssh_get_key(uint32_t key_id, root_ssh_key_t *key)
{
    if (!key)
    {
        return -1;
    }
    return 0;
}

int root_ssh_set_permissions(const char *key_path, uint32_t permissions)
{
    if (!key_path)
    {
        return -1;
    }
    return 0;
}

int root_ssh_get_state(root_ssh_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
