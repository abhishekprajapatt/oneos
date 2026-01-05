#include "x86_lib.h"

int x86_lib_init(void)
{
    return 0;
}

int x86_lib_delay(uint32_t microseconds)
{
    if (microseconds == 0)
    {
        return -1;
    }
    return 0;
}

int x86_lib_halt_cpu(void)
{
    return 0;
}

int x86_lib_reboot_system(void)
{
    return 0;
}

int x86_lib_get_cpuid(uint32_t feature_id, uint32_t *eax, uint32_t *ebx, uint32_t *ecx, uint32_t *edx)
{
    if (!eax || !ebx || !ecx || !edx)
    {
        return -1;
    }
    return 0;
}

int x86_lib_enable_interrupts(void)
{
    return 0;
}

int x86_lib_disable_interrupts(void)
{
    return 0;
}

int x86_lib_get_config(x86_lib_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}
