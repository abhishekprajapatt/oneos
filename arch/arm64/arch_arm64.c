#include "arch_arm64.h"

int arch_arm64_init(void)
{
    return 0;
}

int arch_arm64_save_context(arch_arm64_context_t *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int arch_arm64_restore_context(arch_arm64_context_t *ctx)
{
    if (!ctx)
    {
        return -1;
    }
    return 0;
}

int arch_arm64_context_switch(arch_arm64_context_t *old_ctx, arch_arm64_context_t *new_ctx)
{
    if (!old_ctx || !new_ctx)
    {
        return -1;
    }
    return 0;
}

int arch_arm64_get_cpu_info(arch_arm64_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}

int arch_arm64_enable_interrupt(uint32_t irq)
{
    return 0;
}

int arch_arm64_disable_interrupt(uint32_t irq)
{
    return 0;
}
