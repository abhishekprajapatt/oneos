#include "boot.h"

static boot_info_t boot_current_info = {0, 0, 0};

int boot_init(void)
{
    // Call assembly boot initialization
    extern int boot_x86_init(void);

    boot_current_info.memory_size = 0x80000000;
    boot_current_info.cpu_count = 4;
    boot_current_info.boot_device = 0;

    return 0;
}

int boot_enable_paging(void)
{
    // Call assembly paging setup
    extern void enable_paging(void *page_table);
    enable_paging((void *)0x100000); // Example page table address
    return 0;
}

int boot_load_kernel(const char *path)
{
    if (!path)
    {
        return -1;
    }
    return 0;
}

int boot_get_info(boot_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    info->memory_size = boot_current_info.memory_size;
    info->cpu_count = boot_current_info.cpu_count;
    info->boot_device = boot_current_info.boot_device;
    return 0;
}

int boot_early_print(const char *msg)
{
    if (!msg)
    {
        return -1;
    }
    return 0;
}

int boot_setup_framebuffer(void)
{
    return 0;
}
