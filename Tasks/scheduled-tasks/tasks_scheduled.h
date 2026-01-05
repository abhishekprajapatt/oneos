#ifndef TASKS_SCHEDULED_H
#define TASKS_SCHEDULED_H

#include <stdint.h>

typedef struct
{
    uint32_t task_id;
    const char *task_name;
    const char *task_command;
    const char *schedule;
    uint32_t task_status;
} tasks_scheduled_entry_t;

typedef struct
{
    uint32_t total_scheduled_tasks;
    uint32_t running_tasks;
    uint32_t completed_tasks;
    uint32_t failed_tasks;
} tasks_scheduled_state_t;

int tasks_scheduled_init(void);
int tasks_scheduled_create_task(tasks_scheduled_entry_t *task);
int tasks_scheduled_delete_task(uint32_t task_id);
int tasks_scheduled_run_task(uint32_t task_id);
int tasks_scheduled_get_task(uint32_t task_id, tasks_scheduled_entry_t *task);
int tasks_scheduled_list_tasks(tasks_scheduled_entry_t *tasks, uint32_t *count);
int tasks_scheduled_get_state(tasks_scheduled_state_t *state);

#endif
