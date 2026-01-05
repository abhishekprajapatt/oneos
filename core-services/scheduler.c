#include "scheduler.h"

int scheduler_init(void)
{
    return 0;
}

int scheduler_add_task(scheduler_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int scheduler_remove_task(uint64_t pid)
{
    if (pid == 0)
    {
        return -1;
    }
    return 0;
}

int scheduler_reschedule(void)
{
    return 0;
}

int scheduler_get_next_task(scheduler_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int scheduler_set_priority(uint64_t pid, uint32_t priority)
{
    if (pid == 0 || priority == 0)
    {
        return -1;
    }
    return 0;
}

int scheduler_yield_cpu(uint64_t pid)
{
    if (pid == 0)
    {
        return -1;
    }
    return 0;
}
