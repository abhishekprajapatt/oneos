#include "build_makefile.h"

int build_makefile_init(void)
{
    return 0;
}

int build_makefile_execute(build_makefile_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_makefile_clean(const char *makefile_path)
{
    if (!makefile_path)
    {
        return -1;
    }
    return 0;
}

int build_makefile_get_targets(const char *makefile_path, char **targets, uint32_t *count)
{
    if (!makefile_path || !targets || !count)
    {
        return -1;
    }
    return 0;
}

int build_makefile_get_status(uint32_t build_id, build_makefile_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_makefile_get_state(build_makefile_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
