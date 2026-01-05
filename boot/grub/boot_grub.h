#ifndef BOOT_GRUB_H
#define BOOT_GRUB_H

#include <stdint.h>

typedef struct
{
    uint32_t menu_id;
    const char *menu_name;
    const char *kernel_path;
    const char *kernel_args;
    uint32_t menu_index;
} boot_grub_menu_t;

typedef struct
{
    uint32_t total_menu_entries;
    uint32_t default_entry;
    uint32_t timeout_seconds;
    uint32_t boot_status;
} boot_grub_state_t;

int boot_grub_init(void);
int boot_grub_add_menu_entry(boot_grub_menu_t *menu);
int boot_grub_remove_menu_entry(uint32_t menu_id);
int boot_grub_set_default_entry(uint32_t menu_id);
int boot_grub_get_menu_entry(uint32_t menu_id, boot_grub_menu_t *menu);
int boot_grub_update_config(void);
int boot_grub_get_state(boot_grub_state_t *state);

#endif
