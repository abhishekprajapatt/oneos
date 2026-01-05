#include "sbin_poweroff.h"

int sbin_poweroff_init(void)
{
    return 0;
}

int sbin_poweroff_system(sbin_poweroff_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_poweroff_cancel(void)
{
    return 0;
}

int sbin_poweroff_get_status(uint32_t poweroff_id, sbin_poweroff_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_poweroff_get_state(sbin_poweroff_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
