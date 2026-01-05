#include "sbin_halt.h"

int sbin_halt_init(void)
{
    return 0;
}

int sbin_halt_system(sbin_halt_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_halt_cancel(void)
{
    return 0;
}

int sbin_halt_get_status(uint32_t halt_id, sbin_halt_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_halt_get_state(sbin_halt_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
