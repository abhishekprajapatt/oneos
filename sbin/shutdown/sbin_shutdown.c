#include "sbin_shutdown.h"

int sbin_shutdown_init(void)
{
    return 0;
}

int sbin_shutdown_schedule(sbin_shutdown_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_shutdown_cancel(void)
{
    return 0;
}

int sbin_shutdown_get_status(uint32_t shutdown_id, sbin_shutdown_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_shutdown_get_state(sbin_shutdown_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
