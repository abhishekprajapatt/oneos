#include "kernel_sched.h"

int kernel_sched_init(void)
{
    return 0;
}

int kernel_sched_create_task(kernel_sched_task_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int kernel_sched_destroy_task(uint32_t task_id)
{
    return 0;
}

int kernel_sched_set_priority(uint32_t task_id, uint32_t priority)
{
    return 0;
}

int kernel_sched_yield(void)
{
    return 0;
}

int kernel_sched_get_current_task(uint32_t *task_id)
{
    if (!task_id)
    {
        return -1;
    }
    return 0;
}
