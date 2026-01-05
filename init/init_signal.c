#include "init_signal.h"

int init_signal_register_handler(init_signal_handler_t *handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int init_signal_unregister_handler(uint32_t signal_id)
{
    return 0;
}

int init_signal_send_signal(uint32_t pid, uint32_t signal)
{
    return 0;
}

int init_signal_wait_signal(uint32_t *signal, uint32_t timeout)
{
    if (!signal)
    {
        return -1;
    }
    return 0;
}

int init_signal_block_signal(uint32_t signal)
{
    return 0;
}

int init_signal_unblock_signal(uint32_t signal)
{
    return 0;
}
