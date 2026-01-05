#include "sbin_swapon.h"

int sbin_swapon_init(void)
{
    return 0;
}

int sbin_swapon_enable(sbin_swapon_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_swapon_verify(const char *swap_device)
{
    if (!swap_device)
    {
        return -1;
    }
    return 0;
}

int sbin_swapon_get_status(uint32_t swapon_id, sbin_swapon_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_swapon_get_state(sbin_swapon_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
