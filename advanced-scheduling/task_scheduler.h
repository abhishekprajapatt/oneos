#ifndef TASK_SCHEDULER_H
#define TASK_SCHEDULER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TASKS 4096
#define MAX_PRIORITY_LEVELS 32
#define MAX_CPU_CORES 128
#define MAX_SCHEDULING_POLICIES 16

typedef enum
{
    TASK_STATE_CREATED,
    TASK_STATE_READY,
    TASK_STATE_RUNNING,
    TASK_STATE_WAITING,
    TASK_STATE_BLOCKED,
    TASK_STATE_SUSPENDED,
    TASK_STATE_TERMINATED
} TaskState;

typedef enum
{
    SCHED_FIFO,
    SCHED_RR,
    SCHED_PRIORITY,
    SCHED_DEADLINE,
    SCHED_FAIR,
    SCHED_CFS
} SchedulingPolicy;

typedef struct
{
    uint32_t task_id;
    char task_name[64];
    uint32_t priority;
    TaskState state;
    uint32_t cpu_affinity;

    uint64_t creation_time;
    uint64_t start_time;
    uint64_t end_time;
    uint64_t cpu_time_ms;
    uint64_t wait_time_ms;

    uint32_t preemption_count;
    uint32_t context_switches;
    float cpu_utilization;
} Task;

typedef struct
{
    uint32_t runqueue_id;
    uint32_t cpu_core;

    uint32_t task_count;
    Task *tasks[MAX_TASKS];

    uint32_t current_task_index;
    uint64_t time_slice_ms;

    uint64_t total_exec_time;
    uint64_t context_switch_count;
    float average_utilization;
} RunQueue;

typedef struct
{
    uint32_t scheduler_id;
    SchedulingPolicy policy;

    uint32_t core_count;
    RunQueue runqueues[MAX_CPU_CORES];

    uint32_t total_tasks;
    Task tasks[MAX_TASKS];

    uint64_t total_scheduling_decisions;
    uint64_t total_preemptions;
    float average_response_time;
    float average_turnaround_time;
} TaskScheduler;

// Scheduler initialization
TaskScheduler *scheduler_init(SchedulingPolicy policy, uint32_t core_count);
int scheduler_destroy(TaskScheduler *sched);

// Task creation and management
Task *scheduler_create_task(TaskScheduler *sched, const char *name, uint32_t priority);
int scheduler_destroy_task(TaskScheduler *sched, uint32_t task_id);
int scheduler_set_task_priority(TaskScheduler *sched, uint32_t task_id, uint32_t priority);
int scheduler_set_cpu_affinity(TaskScheduler *sched, uint32_t task_id, uint32_t cpu_mask);

// Task state transitions
int scheduler_enqueue_task(TaskScheduler *sched, uint32_t task_id);
int scheduler_dequeue_task(TaskScheduler *sched, uint32_t runqueue_id);
int scheduler_suspend_task(TaskScheduler *sched, uint32_t task_id);
int scheduler_resume_task(TaskScheduler *sched, uint32_t task_id);
int scheduler_terminate_task(TaskScheduler *sched, uint32_t task_id);

// Scheduling operations
int scheduler_run_scheduling_cycle(TaskScheduler *sched);
Task *scheduler_select_next_task(TaskScheduler *sched, uint32_t cpu_core);
int scheduler_context_switch(TaskScheduler *sched, uint32_t from_task_id, uint32_t to_task_id);

// Load balancing
int scheduler_balance_load(TaskScheduler *sched);
int scheduler_migrate_task(TaskScheduler *sched, uint32_t task_id, uint32_t target_core);

// Policy management
int scheduler_change_policy(TaskScheduler *sched, SchedulingPolicy new_policy);
int scheduler_set_time_slice(TaskScheduler *sched, uint64_t time_slice_ms);

// Monitoring
typedef struct
{
    uint32_t scheduler_id;
    uint32_t active_tasks;
    uint32_t total_tasks;
    float average_response_time_ms;
    float average_turnaround_time_ms;
    uint64_t total_context_switches;
    float average_cpu_utilization;
    float scheduler_efficiency;
} SchedulerMetrics;

int scheduler_get_metrics(TaskScheduler *sched, SchedulerMetrics *metrics);
int scheduler_get_task_metrics(TaskScheduler *sched, uint32_t task_id);

#endif
