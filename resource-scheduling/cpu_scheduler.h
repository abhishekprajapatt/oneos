#ifndef CPU_SCHEDULER_H
#define CPU_SCHEDULER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_SCHEDULABLE_THREADS 4096
#define MAX_RUNQUEUES 256
#define MAX_SCHEDULER_DOMAINS 128
#define MAX_MIGRATION_CANDIDATES 512

typedef enum
{
    SCHED_POLICY_FIFO,
    SCHED_POLICY_ROUND_ROBIN,
    SCHED_POLICY_CFS,
    SCHED_POLICY_RT,
    SCHED_POLICY_DEADLINE
} SchedulingPolicy;

typedef enum
{
    THREAD_STATE_RUNNABLE,
    THREAD_STATE_RUNNING,
    THREAD_STATE_WAITING,
    THREAD_STATE_BLOCKED,
    THREAD_STATE_SLEEPING
} ThreadState;

typedef struct
{
    uint32_t thread_id;
    uint32_t process_id;
    int priority;
    ThreadState state;

    uint64_t cpu_time_ns;
    uint64_t vruntime_ns;
    uint32_t time_slice_ms;
    uint32_t cpu_affinity;

    uint32_t context_switches;
    uint32_t total_schedules;
} ScheduleEntry;

typedef struct
{
    uint32_t cpu_id;
    uint32_t load;
    ScheduleEntry queue[MAX_SCHEDULABLE_THREADS];
    uint32_t queue_length;

    uint64_t total_context_switches;
    uint64_t idle_time_ns;
    uint64_t active_time_ns;
} RunQueue;

typedef struct
{
    uint32_t domain_id;
    uint32_t cpus[64];
    uint32_t cpu_count;

    uint32_t load_balance_interval_ms;
    uint64_t last_balance_time;
} SchedulerDomain;

typedef struct
{
    uint32_t scheduler_id;
    SchedulingPolicy policy;

    RunQueue runqueues[MAX_RUNQUEUES];
    uint32_t runqueue_count;

    ScheduleEntry threads[MAX_SCHEDULABLE_THREADS];
    uint32_t thread_count;

    SchedulerDomain domains[MAX_SCHEDULER_DOMAINS];
    uint32_t domain_count;

    uint32_t total_threads_scheduled;
    uint32_t total_migrations;
    uint32_t load_balancing_operations;
} CPUScheduler;

CPUScheduler *cpu_scheduler_init(SchedulingPolicy policy, uint32_t cpu_count);
int cpu_scheduler_destroy(CPUScheduler *scheduler);

int cpu_scheduler_add_runqueue(CPUScheduler *scheduler, uint32_t cpu_id);
int cpu_scheduler_remove_runqueue(CPUScheduler *scheduler, uint32_t cpu_id);

uint32_t cpu_scheduler_enqueue_thread(CPUScheduler *scheduler, uint32_t thread_id,
                                      uint32_t process_id, int priority, uint32_t cpu_affinity);
int cpu_scheduler_dequeue_thread(CPUScheduler *scheduler, uint32_t cpu_id, uint32_t thread_id);

uint32_t cpu_scheduler_select_next_thread(CPUScheduler *scheduler, uint32_t cpu_id);
int cpu_scheduler_run_thread(CPUScheduler *scheduler, uint32_t cpu_id, uint32_t thread_id);

int cpu_scheduler_update_vruntime(CPUScheduler *scheduler, uint32_t thread_id, uint64_t runtime_ns);
int cpu_scheduler_check_need_resched(CPUScheduler *scheduler, uint32_t cpu_id);

int cpu_scheduler_balance_load(CPUScheduler *scheduler, uint32_t domain_id);
int cpu_scheduler_migrate_thread(CPUScheduler *scheduler, uint32_t thread_id, uint32_t target_cpu);

int cpu_scheduler_find_load_balanced_cpu(CPUScheduler *scheduler, uint32_t thread_id);
int cpu_scheduler_update_runqueue_loads(CPUScheduler *scheduler);

int cpu_scheduler_wake_up_thread(CPUScheduler *scheduler, uint32_t thread_id);
int cpu_scheduler_sleep_thread(CPUScheduler *scheduler, uint32_t thread_id);

typedef struct
{
    uint32_t scheduler_id;
    uint32_t total_threads;
    uint32_t runnable_threads;
    uint32_t running_threads;
    uint32_t sleeping_threads;
    uint32_t total_migrations;
    float average_load;
    float scheduling_efficiency;
} CPUSchedulerMetrics;

int cpu_scheduler_get_metrics(CPUScheduler *scheduler, CPUSchedulerMetrics *metrics);

#endif
