#include "sbin_grubmkrescue.h"

int sbin_grubmkrescue_init(void)
{
    return 0;
}

int sbin_grubmkrescue_create(sbin_grubmkrescue_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_grubmkrescue_validate(const char *iso_path)
{
    if (!iso_path)
    {
        return -1;
    }
    return 0;
}

int sbin_grubmkrescue_get_status(uint32_t rescue_id, sbin_grubmkrescue_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_grubmkrescue_get_state(sbin_grubmkrescue_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
