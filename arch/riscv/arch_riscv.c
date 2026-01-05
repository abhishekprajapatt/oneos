#include "arch_riscv.h"

int arch_riscv_init(void)
{
    return 0;
}

int arch_riscv_save_context(arch_riscv_context_t *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int arch_riscv_restore_context(arch_riscv_context_t *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int arch_riscv_context_switch(arch_riscv_context_t *old_ctx, arch_riscv_context_t *new_ctx)
{
    if (!old_ctx || !new_ctx)
    {
        return -1;
    }
    return 0;
}

int arch_riscv_get_cpu_info(arch_riscv_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}

int arch_riscv_enable_interrupt(uint32_t irq)
{
    return 0;
}

int arch_riscv_disable_interrupt(uint32_t irq)
{
    return 0;
}
