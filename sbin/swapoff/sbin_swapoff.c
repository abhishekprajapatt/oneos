#include "sbin_swapoff.h"

int sbin_swapoff_init(void)
{
    return 0;
}

int sbin_swapoff_disable(sbin_swapoff_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_swapoff_verify(const char *swap_device)
{
    if (!swap_device)
    {
        return -1;
    }
    return 0;
}

int sbin_swapoff_get_status(uint32_t swapoff_id, sbin_swapoff_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_swapoff_get_state(sbin_swapoff_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
