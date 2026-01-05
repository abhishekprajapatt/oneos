#ifndef ARCH_ARM64_BOOT_H
#define ARCH_ARM64_BOOT_H

#include <stdint.h>

typedef struct
{
    uint64_t entry_point;
    uint64_t dtb_address;
    uint32_t bootloader_version;
    uint32_t boot_status;
} arm64_boot_params_t;

typedef struct
{
    uint32_t total_boots;
    uint32_t successful_boots;
    uint32_t failed_boots;
    uint64_t total_boot_time;
} arm64_boot_stats_t;

int arm64_boot_init(void);
int arm64_boot_setup_mmu(void);
int arm64_boot_enable_caches(void);
int arm64_boot_initialize_gic(void);
int arm64_boot_setup_stack(uint64_t stack_base, uint64_t stack_size);
int arm64_boot_jump_to_kernel(uint64_t kernel_entry);
int arm64_boot_get_stats(arm64_boot_stats_t *stats);

#endif
