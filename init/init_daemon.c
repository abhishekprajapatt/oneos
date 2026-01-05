#include "init_daemon.h"

int init_daemon_spawn(init_daemon_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}

int init_daemon_kill(uint32_t daemon_id)
{
    return 0;
}

int init_daemon_respawn(uint32_t daemon_id)
{
    return 0;
}

int init_daemon_wait(uint32_t daemon_id, uint32_t timeout)
{
    return 0;
}

int init_daemon_signal(uint32_t daemon_id, uint32_t signal)
{
    return 0;
}

int init_daemon_get_info(uint32_t daemon_id, init_daemon_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}
