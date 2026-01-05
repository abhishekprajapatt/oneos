#ifndef ARCH_X86_BOOT_H
#define ARCH_X86_BOOT_H

#include <stdint.h>

typedef struct
{
    uint32_t entry_point;
    uint32_t multiboot_magic;
    uint32_t bootloader_version;
    uint32_t boot_flags;
} x86_boot_params_t;

typedef struct
{
    uint32_t total_boots;
    uint32_t successful_boots;
    uint32_t failed_boots;
    uint64_t total_boot_time;
} x86_boot_stats_t;

int x86_boot_init(void);
int x86_boot_setup_gdt(void);
int x86_boot_setup_idt(void);
int x86_boot_enable_paging(void);
int x86_boot_setup_pae(void);
int x86_boot_jump_to_kernel(uint32_t kernel_entry);
int x86_boot_get_stats(x86_boot_stats_t *stats);

#endif
