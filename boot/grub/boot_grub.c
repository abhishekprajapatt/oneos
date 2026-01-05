#include "boot_grub.h"

int boot_grub_init(void)
{
    return 0;
}

int boot_grub_add_menu_entry(boot_grub_menu_t *menu)
{
    if (!menu)
    {
        return -1;
    }
    return 0;
}

int boot_grub_remove_menu_entry(uint32_t menu_id)
{
    return 0;
}

int boot_grub_set_default_entry(uint32_t menu_id)
{
    return 0;
}

int boot_grub_get_menu_entry(uint32_t menu_id, boot_grub_menu_t *menu)
{
    if (!menu)
    {
        return -1;
    }
    return 0;
}

int boot_grub_update_config(void)
{
    return 0;
}

int boot_grub_get_state(boot_grub_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
