#ifndef ARCH_RISCV_CPU_H
#define ARCH_RISCV_CPU_H

#include <stdint.h>

typedef struct
{
    uint32_t hart_id;
    uint32_t online;
    const char *isa_extension;
    uint64_t frequency;
} riscv_hart_info_t;

typedef struct
{
    uint32_t total_harts;
    uint32_t online_harts;
    uint32_t offline_harts;
    const char *priv_level;
} riscv_hart_stats_t;

int riscv_hart_init(void);
int riscv_hart_online(uint32_t hart_id);
int riscv_hart_offline(uint32_t hart_id);
int riscv_hart_get_info(uint32_t hart_id, riscv_hart_info_t *info);
int riscv_hart_set_frequency(uint32_t hart_id, uint64_t frequency);
int riscv_hart_send_ipi(uint32_t hart_id, uint32_t interrupt_type);
int riscv_hart_get_stats(riscv_hart_stats_t *stats);

#endif
