#include "build_linker.h"

int build_linker_init(void)
{
    return 0;
}

int build_linker_link(build_linker_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_linker_get_symbols(uint32_t link_id, char *symbols, uint32_t *count)
{
    if (!symbols || !count)
    {
        return -1;
    }
    return 0;
}

int build_linker_cancel(uint32_t link_id)
{
    return 0;
}

int build_linker_get_status(uint32_t link_id, build_linker_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_linker_get_state(build_linker_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
