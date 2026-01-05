#include "build_compiler.h"

int build_compiler_init(void)
{
    return 0;
}

int build_compiler_compile(build_compiler_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_compiler_get_errors(uint32_t compile_id, char *errors, uint32_t *size)
{
    if (!errors || !size)
    {
        return -1;
    }
    return 0;
}

int build_compiler_cancel(uint32_t compile_id)
{
    return 0;
}

int build_compiler_get_status(uint32_t compile_id, build_compiler_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_compiler_get_state(build_compiler_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
