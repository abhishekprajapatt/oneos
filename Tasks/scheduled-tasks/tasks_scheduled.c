#include "tasks_scheduled.h"

int tasks_scheduled_init(void)
{
    return 0;
}

int tasks_scheduled_create_task(tasks_scheduled_entry_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int tasks_scheduled_delete_task(uint32_t task_id)
{
    return 0;
}

int tasks_scheduled_run_task(uint32_t task_id)
{
    return 0;
}

int tasks_scheduled_get_task(uint32_t task_id, tasks_scheduled_entry_t *task)
{
    if (!task)
    {
        return -1;
    }
    return 0;
}

int tasks_scheduled_list_tasks(tasks_scheduled_entry_t *tasks, uint32_t *count)
{
    if (!tasks || !count)
    {
        return -1;
    }
    return 0;
}

int tasks_scheduled_get_state(tasks_scheduled_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
