#include "etc_init.h"

int etc_init_register_script(etc_init_script_t *script)
{
    if (!script)
    {
        return -1;
    }
    return 0;
}

int etc_init_unregister_script(uint32_t script_id)
{
    return 0;
}

int etc_init_execute_script(uint32_t script_id, const char *action)
{
    if (!action)
    {
        return -1;
    }
    return 0;
}

int etc_init_set_runlevel(uint32_t runlevel)
{
    return 0;
}

int etc_init_get_runlevel(uint32_t *runlevel)
{
    if (!runlevel)
    {
        return -1;
    }
    return 0;
}

int etc_init_list_scripts(uint32_t runlevel, etc_init_script_t *scripts, uint32_t max_count)
{
    if (!scripts)
    {
        return -1;
    }
    return 0;
}
