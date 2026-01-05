#include "sbin_grubinstall.h"

int sbin_grubinstall_init(void)
{
    return 0;
}

int sbin_grubinstall_install(sbin_grubinstall_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_grubinstall_check(const char *boot_device)
{
    if (!boot_device)
    {
        return -1;
    }
    return 0;
}

int sbin_grubinstall_get_status(uint32_t install_id, sbin_grubinstall_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int sbin_grubinstall_get_state(sbin_grubinstall_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
