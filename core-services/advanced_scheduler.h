#ifndef ADVANCED_SCHEDULER_H
#define ADVANCED_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_CPU_CORES 256
#define MAX_SCHEDULING_DOMAINS 16
#define MAX_TASKS_PER_DOMAIN 1024

typedef enum
{
    SCHED_CLASS_REALTIME = 0,
    SCHED_CLASS_DEADLINE = 1,
    SCHED_CLASS_FAIR = 2,
    SCHED_CLASS_IDLE = 3
} SchedulingClass;

typedef struct
{
    uint32_t task_id;
    uint32_t cpu_affinity_mask;
    uint32_t nice;
    int deadline_ms;
    SchedulingClass sched_class;
    uint32_t cpu_time_ms;
    uint32_t preemption_count;
    bool is_pinned;
    uint32_t preferred_core;
} TaskInfo;

typedef struct
{
    uint32_t core_id;
    uint32_t task_count;
    uint64_t total_time;
    uint64_t idle_time;
    double utilization;
    uint32_t current_task_id;
    uint32_t context_switches;
} CPUCore;

typedef struct
{
    uint32_t domain_id;
    uint32_t core_mask;
    uint32_t core_count;
    TaskInfo tasks[MAX_TASKS_PER_DOMAIN];
    uint32_t task_count;
    double load_balance_factor;
} SchedulingDomain;

typedef struct
{
    CPUCore cores[MAX_CPU_CORES];
    uint32_t core_count;
    SchedulingDomain domains[MAX_SCHEDULING_DOMAINS];
    uint32_t domain_count;
    TaskInfo task_table[4096];
    uint32_t task_count;
    uint64_t scheduling_events;
    double avg_load;
} AdvancedScheduler;

AdvancedScheduler *scheduler_init(uint32_t core_count);
void scheduler_cleanup(AdvancedScheduler *sched);

uint32_t scheduler_add_task(AdvancedScheduler *sched, uint32_t task_id,
                            uint32_t cpu_affinity, SchedulingClass class, int deadline);
int scheduler_remove_task(AdvancedScheduler *sched, uint32_t task_id);
int scheduler_set_affinity(AdvancedScheduler *sched, uint32_t task_id, uint32_t affinity_mask);
int scheduler_pin_task(AdvancedScheduler *sched, uint32_t task_id, uint32_t core);

int scheduler_load_balance(AdvancedScheduler *sched);
uint32_t scheduler_select_cpu(AdvancedScheduler *sched, uint32_t task_id);
int scheduler_create_domain(AdvancedScheduler *sched, uint32_t core_mask);

int scheduler_update_load(AdvancedScheduler *sched);
double scheduler_get_cpu_load(AdvancedScheduler *sched, uint32_t core_id);
double scheduler_get_system_load(AdvancedScheduler *sched);

TaskInfo *scheduler_get_task_info(AdvancedScheduler *sched, uint32_t task_id);
CPUCore *scheduler_get_core_info(AdvancedScheduler *sched, uint32_t core_id);
uint32_t scheduler_get_task_count(AdvancedScheduler *sched);

#endif
