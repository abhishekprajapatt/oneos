#ifndef KERNEL_SCHED_H
#define KERNEL_SCHED_H

#include <stdint.h>

typedef struct
{
    uint32_t task_id;
    uint32_t priority;
    uint32_t state;
    uint64_t runtime_ns;
} kernel_sched_task_t;

typedef struct
{
    uint32_t task_count;
    uint32_t running_tasks;
    uint32_t waiting_tasks;
} kernel_sched_state_t;

int kernel_sched_init(void);
int kernel_sched_create_task(kernel_sched_task_t *task);
int kernel_sched_destroy_task(uint32_t task_id);
int kernel_sched_set_priority(uint32_t task_id, uint32_t priority);
int kernel_sched_yield(void);
int kernel_sched_get_current_task(uint32_t *task_id);

#endif
