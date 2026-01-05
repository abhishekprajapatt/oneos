#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdint.h>

typedef struct
{
    uint64_t pid;
    uint32_t priority;
    uint32_t state;
    uint64_t cpu_time;
} scheduler_task_t;

int scheduler_init(void);
int scheduler_add_task(scheduler_task_t *task);
int scheduler_remove_task(uint64_t pid);
int scheduler_reschedule(void);
int scheduler_get_next_task(scheduler_task_t *task);
int scheduler_set_priority(uint64_t pid, uint32_t priority);
int scheduler_yield_cpu(uint64_t pid);

#endif
