#include "riscv.h"

int riscv_init(void)
{
    return 0;
}

int riscv_setup_trap_vector(uint64_t trap_vec)
{
    if (trap_vec == 0)
    {
        return -1;
    }
    return 0;
}

int riscv_enable_interrupts(void)
{
    return 0;
}

int riscv_disable_interrupts(void)
{
    return 0;
}

int riscv_set_privilege_level(uint32_t level)
{
    if (level > 3)
    {
        return -1;
    }
    return 0;
}

int riscv_context_switch(void *old_ctx, void *new_ctx)
{
    if (!old_ctx || !new_ctx)
    {
        return -1;
    }
    return 0;
}
