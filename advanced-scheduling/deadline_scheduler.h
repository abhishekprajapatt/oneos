#ifndef DEADLINE_SCHEDULER_H
#define DEADLINE_SCHEDULER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_DEADLINE_TASKS 2048
#define MAX_PERIODIC_TASKS 512
#define MAX_APERIODIC_TASKS 1024

typedef enum
{
    TASK_PERIODIC,
    TASK_APERIODIC,
    TASK_SPORADIC
} TaskType;

typedef struct
{
    uint32_t deadline_task_id;
    TaskType task_type;

    uint64_t arrival_time;
    uint64_t absolute_deadline;
    uint64_t relative_deadline;
    uint64_t period;

    uint64_t computation_time;
    uint64_t actual_start_time;
    uint64_t actual_end_time;

    uint8_t is_feasible;
    uint8_t is_completed;
    uint8_t missed_deadline;

    uint32_t priority;
} DeadlineTask;

typedef struct
{
    uint32_t scheduler_id;

    uint32_t periodic_task_count;
    DeadlineTask periodic_tasks[MAX_PERIODIC_TASKS];

    uint32_t aperiodic_task_count;
    DeadlineTask aperiodic_tasks[MAX_APERIODIC_TASKS];

    uint64_t total_deadline_tasks;
    uint64_t tasks_completed_on_time;
    uint64_t tasks_missed_deadline;

    float deadline_miss_ratio;
    float average_slack_time;
    float cpu_utilization;
} DeadlineScheduler;

DeadlineScheduler *deadline_scheduler_init(void);
int deadline_scheduler_destroy(DeadlineScheduler *sched);

DeadlineTask *deadline_add_periodic_task(DeadlineScheduler *sched, uint64_t arrival, uint64_t deadline, uint64_t period, uint64_t computation);
DeadlineTask *deadline_add_aperiodic_task(DeadlineScheduler *sched, uint64_t arrival, uint64_t deadline, uint64_t computation);

int deadline_scheduler_run_cycle(DeadlineScheduler *sched);
int deadline_check_feasibility(DeadlineScheduler *sched);
int deadline_update_task_status(DeadlineScheduler *sched, uint32_t task_id);

int deadline_compute_slack_time(DeadlineScheduler *sched);
int deadline_compute_laxity(DeadlineTask *task, uint64_t current_time);

int deadline_detect_deadline_miss(DeadlineScheduler *sched);

typedef struct
{
    uint32_t scheduler_id;
    uint64_t total_tasks;
    uint64_t tasks_on_time;
    uint64_t tasks_missed;
    float miss_ratio;
    float average_slack;
    float utilization;
} DeadlineMetrics;

int deadline_get_metrics(DeadlineScheduler *sched, DeadlineMetrics *metrics);

#endif
