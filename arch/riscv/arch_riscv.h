#ifndef ARCH_RISCV_H
#define ARCH_RISCV_H

#include <stdint.h>

typedef struct
{
    uint64_t ra;
    uint64_t sp;
    uint64_t gp;
    uint64_t tp;
    uint64_t t0;
    uint64_t t1;
    uint64_t t2;
    uint64_t s0;
    uint64_t pc;
} arch_riscv_context_t;

typedef struct
{
    uint32_t cpu_count;
    uint32_t cpu_freq;
    uint64_t l1_cache_size;
    uint64_t l2_cache_size;
    uint32_t isa_version;
} arch_riscv_state_t;

int arch_riscv_init(void);
int arch_riscv_save_context(arch_riscv_context_t *ctx);
int arch_riscv_restore_context(arch_riscv_context_t *ctx);
int arch_riscv_context_switch(arch_riscv_context_t *old_ctx, arch_riscv_context_t *new_ctx);
int arch_riscv_get_cpu_info(arch_riscv_state_t *state);
int arch_riscv_enable_interrupt(uint32_t irq);
int arch_riscv_disable_interrupt(uint32_t irq);

#endif
