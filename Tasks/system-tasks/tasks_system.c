#include "tasks_system.h"

int tasks_system_init(void)
{
    return 0;
}

int tasks_system_submit_task(tasks_system_entry_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int tasks_system_cancel_task(uint32_t task_id)
{
    return 0;
}

int tasks_system_get_task(uint32_t task_id, tasks_system_entry_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int tasks_system_get_status(uint32_t task_id, uint32_t *status)
{
    if (!status)
    {
        return -1;
    }
    return 0;
}

int tasks_system_list_tasks(uint32_t priority, tasks_system_entry_t *tasks, uint32_t *count)
{
    if (!tasks || !count)
    {
        return -1;
    }
    return 0;
}

int tasks_system_get_state(tasks_system_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
