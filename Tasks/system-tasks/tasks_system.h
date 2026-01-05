#ifndef TASKS_SYSTEM_H
#define TASKS_SYSTEM_H

#include <stdint.h>

typedef struct
{
    uint32_t task_id;
    const char *task_name;
    const char *task_type;
    uint32_t task_priority;
    uint32_t task_status;
} tasks_system_entry_t;

typedef struct
{
    uint32_t total_system_tasks;
    uint32_t running_system_tasks;
    uint32_t pending_system_tasks;
    uint32_t completed_system_tasks;
} tasks_system_state_t;

int tasks_system_init(void);
int tasks_system_submit_task(tasks_system_entry_t *task);
int tasks_system_cancel_task(uint32_t task_id);
int tasks_system_get_task(uint32_t task_id, tasks_system_entry_t *task);
int tasks_system_get_status(uint32_t task_id, uint32_t *status);
int tasks_system_list_tasks(uint32_t priority, tasks_system_entry_t *tasks, uint32_t *count);
int tasks_system_get_state(tasks_system_state_t *state);

#endif
