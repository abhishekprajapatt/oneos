#include "arch_x86.h"

int arch_x86_init(void)
{
    return 0;
}

int arch_x86_save_context(arch_x86_context_t *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int arch_x86_restore_context(arch_x86_context_t *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int arch_x86_context_switch(arch_x86_context_t *old_ctx, arch_x86_context_t *new_ctx)
{
    if (!old_ctx || !new_ctx)
    {
        return -1;
    }
    return 0;
}

int arch_x86_get_cpu_info(arch_x86_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}

int arch_x86_enable_interrupt(uint32_t irq)
{
    return 0;
}

int arch_x86_disable_interrupt(uint32_t irq)
{
    return 0;
}
