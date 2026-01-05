#include "build_package.h"

int build_package_init(void)
{
    return 0;
}

int build_package_create(build_package_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_package_get_contents(uint32_t package_id, char *contents, uint32_t *size)
{
    if (!contents || !size)
    {
        return -1;
    }
    return 0;
}

int build_package_verify(uint32_t package_id)
{
    return 0;
}

int build_package_get_status(uint32_t package_id, build_package_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int build_package_get_state(build_package_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
