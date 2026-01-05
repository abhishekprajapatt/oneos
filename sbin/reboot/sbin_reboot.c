#include "sbin_reboot.h"

int sbin_reboot_init(void)
{
    return 0;
}

int sbin_reboot_system(sbin_reboot_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_reboot_cancel(void)
{
    return 0;
}

int sbin_reboot_get_status(uint32_t reboot_id, sbin_reboot_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_reboot_get_state(sbin_reboot_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
