#include "etc_ssh.h"

int etc_ssh_init(void)
{
    return 0;
}

int etc_ssh_load_config(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}

int etc_ssh_set_config(etc_ssh_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}

int etc_ssh_get_server_config(etc_ssh_server_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}

int etc_ssh_set_server_config(etc_ssh_server_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}

int etc_ssh_save_config(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}
