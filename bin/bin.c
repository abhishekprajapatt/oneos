#include "bin.h"

int bin_init(void)
{
    return 0;
}

int bin_exec(const char *program, const char **args)
{
    if (!program)
    {
        return -1;
    }
    return 0;
}

int bin_spawn_process(const char *program, const char **args, uint32_t *pid)
{
    if (!program || !pid)
    {
        return -1;
    }
    return 0;
}

int bin_wait_process(uint32_t pid)
{
    if (pid == 0)
    {
        return -1;
    }
    return 0;
}

int bin_kill_process(uint32_t pid, uint32_t signal)
{
    if (pid == 0)
    {
        return -1;
    }
    return 0;
}
