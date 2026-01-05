#ifndef ARCH_X86_H
#define ARCH_X86_H

#include <stdint.h>

typedef struct
{
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
    uint32_t esi;
    uint32_t edi;
    uint32_t esp;
    uint32_t ebp;
    uint32_t eip;
    uint32_t eflags;
} arch_x86_context_t;

typedef struct
{
    uint32_t cpu_count;
    uint32_t cpu_freq;
    uint64_t l1_cache_size;
    uint64_t l2_cache_size;
    uint32_t cpuid_features;
} arch_x86_state_t;

int arch_x86_init(void);
int arch_x86_save_context(arch_x86_context_t *ctx);
int arch_x86_restore_context(arch_x86_context_t *ctx);
int arch_x86_context_switch(arch_x86_context_t *old_ctx, arch_x86_context_t *new_ctx);
int arch_x86_get_cpu_info(arch_x86_state_t *state);
int arch_x86_enable_interrupt(uint32_t irq);
int arch_x86_disable_interrupt(uint32_t irq);

#endif
