#include "x86.h"

int x86_init(void)
{
    return 0;
}

int x86_setup_gdt(void)
{
    return 0;
}

int x86_setup_idt(void)
{
    return 0;
}

int x86_enable_paging(void)
{
    return 0;
}

int x86_enable_interrupts(void)
{
    return 0;
}

int x86_disable_interrupts(void)
{
    return 0;
}

int x86_context_switch(void *old_ctx, void *new_ctx)
{
    if (!old_ctx || !new_ctx)
    {
        return -1;
    }
    return 0;
}
