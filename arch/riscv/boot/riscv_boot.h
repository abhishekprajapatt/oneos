#ifndef ARCH_RISCV_BOOT_H
#define ARCH_RISCV_BOOT_H

#include <stdint.h>

typedef struct
{
    uint64_t entry_point;
    uint64_t hart_id;
    uint32_t isa_string_length;
    const char *isa_string;
} riscv_boot_params_t;

typedef struct
{
    uint32_t total_boots;
    uint32_t successful_boots;
    uint32_t failed_boots;
    uint64_t total_boot_time;
} riscv_boot_stats_t;

int riscv_boot_init(void);
int riscv_boot_setup_paging(void);
int riscv_boot_enable_supervisor(void);
int riscv_boot_setup_traps(void);
int riscv_boot_configure_timer(void);
int riscv_boot_jump_to_kernel(uint64_t kernel_entry);
int riscv_boot_get_stats(riscv_boot_stats_t *stats);

#endif
