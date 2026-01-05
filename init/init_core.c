#include "init_core.h"

int init_core_start(void)
{
    return 0;
}

int init_core_shutdown(void)
{
    return 0;
}

int init_core_set_runlevel(uint32_t runlevel)
{
    return 0;
}

int init_core_get_runlevel(uint32_t *runlevel)
{
    if (!runlevel)
    {
        return -1;
    }
    return 0;
}

int init_core_register_process(init_core_process_t *process)
{
    if (!process)
    {
        return -1;
    }
    return 0;
}

int init_core_unregister_process(uint32_t process_id)
{
    return 0;
}
