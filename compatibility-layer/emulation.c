#include "emulation.h"

int emulation_init(void)
{
    return 0;
}

int emulation_set_mode(uint32_t mode)
{
    if (mode == 0)
    {
        return -1;
    }
    return 0;
}

int emulation_load_environment(const char *env_name)
{
    if (!env_name)
    {
        return -1;
    }
    return 0;
}

int emulation_translate_syscall(uint32_t syscall)
{
    if (syscall == 0)
    {
        return -1;
    }
    return 0;
}

int emulation_get_mode(void)
{
    return 0;
}

int emulation_cleanup(void)
{
    return 0;
}
