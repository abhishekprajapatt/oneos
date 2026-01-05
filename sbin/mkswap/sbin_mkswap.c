#include "sbin_mkswap.h"

int sbin_mkswap_init(void)
{
    return 0;
}

int sbin_mkswap_create(sbin_mkswap_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_mkswap_verify(const char *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int sbin_mkswap_get_status(uint32_t swap_id, sbin_mkswap_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_mkswap_get_state(sbin_mkswap_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
