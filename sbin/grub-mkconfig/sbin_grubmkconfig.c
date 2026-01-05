#include "sbin_grubmkconfig.h"

int sbin_grubmkconfig_init(void)
{
    return 0;
}

int sbin_grubmkconfig_generate(sbin_grubmkconfig_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_grubmkconfig_validate(const char *config_path)
{
    if (!config_path)
    {
        return -1;
    }
    return 0;
}

int sbin_grubmkconfig_get_status(uint32_t config_id, sbin_grubmkconfig_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_grubmkconfig_get_state(sbin_grubmkconfig_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
