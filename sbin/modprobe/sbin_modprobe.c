#include "sbin_modprobe.h"

int sbin_modprobe_init(void)
{
    return 0;
}

int sbin_modprobe_load(sbin_modprobe_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_modprobe_unload(const char *module_name)
{
    if (!module_name)
    {
        return -1;
    }
    return 0;
}

int sbin_modprobe_get_status(uint32_t modprobe_id, sbin_modprobe_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_modprobe_get_state(sbin_modprobe_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
