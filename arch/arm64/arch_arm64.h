#ifndef ARCH_ARM64_H
#define ARCH_ARM64_H

#include <stdint.h>

typedef struct
{
    uint64_t x0;
    uint64_t x1;
    uint64_t x2;
    uint64_t x3;
    uint64_t x4;
    uint64_t x5;
    uint64_t x6;
    uint64_t x7;
    uint64_t sp;
    uint64_t pc;
} arch_arm64_context_t;

typedef struct
{
    uint32_t cpu_count;
    uint32_t cpu_freq;
    uint64_t l1_cache_size;
    uint64_t l2_cache_size;
    uint32_t features;
} arch_arm64_state_t;

int arch_arm64_init(void);
int arch_arm64_save_context(arch_arm64_context_t *ctx);
int arch_arm64_restore_context(arch_arm64_context_t *ctx);
int arch_arm64_context_switch(arch_arm64_context_t *old_ctx, arch_arm64_context_t *new_ctx);
int arch_arm64_get_cpu_info(arch_arm64_state_t *state);
int arch_arm64_enable_interrupt(uint32_t irq);
int arch_arm64_disable_interrupt(uint32_t irq);

#endif
