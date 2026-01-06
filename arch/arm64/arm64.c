#include "arm64.h"

int arm64_init(void)
{
    return 0;
}

int arm64_setup_mmu(void)
{
    return 0;
}

int arm64_set_ttbr0(uint64_t ttbr0)
{
    if (ttbr0 == 0)
    {
        return -1;
    }
    return 0;
}

int arm64_enable_interrupts(void)
{
    return 0;
}

int arm64_disable_interrupts(void)
{
    return 0;
}

int arm64_context_switch(void *old_ctx, void *new_ctx)
{
    if (!old_ctx || !new_ctx)
    {
        return -1;
    }
    return 0;
}
