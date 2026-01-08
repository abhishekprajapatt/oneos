#ifndef REALTIME_H
#define REALTIME_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_RT_TASKS 512
#define MAX_RT_TIMERS 256

typedef enum
{
    RT_CLASS_HARD = 0,
    RT_CLASS_SOFT = 1,
    RT_CLASS_FIRM = 2
} RTClass;

typedef enum
{
    RT_STATE_PENDING = 0,
    RT_STATE_READY = 1,
    RT_STATE_RUNNING = 2,
    RT_STATE_WAITING = 3,
    RT_STATE_COMPLETED = 4,
    RT_STATE_MISSED = 5
} RTTaskState;

typedef struct
{
    uint32_t task_id;
    RTClass rt_class;
    RTTaskState state;
    int64_t period_us;
    int64_t deadline_us;
    int64_t worst_case_time_us;
    int64_t actual_runtime_us;
    uint32_t deadline_misses;
    int priority;
    bool is_preemptible;
    time_t created_time;
} RTTask;

typedef struct
{
    uint32_t timer_id;
    int64_t interval_us;
    int64_t remaining_us;
    bool is_periodic;
    bool is_active;
    void (*callback)(uint32_t);
    uint32_t fires;
} RTTimer;

typedef struct
{
    RTTask tasks[MAX_RT_TASKS];
    uint32_t task_count;
    RTTimer timers[MAX_RT_TIMERS];
    uint32_t timer_count;
    uint64_t total_deadline_misses;
    uint64_t total_preemptions;
    double cpu_reservation_percent;
} RealtimeManager;

RealtimeManager *rt_init(void);
void rt_cleanup(RealtimeManager *manager);

uint32_t rt_create_task(RealtimeManager *manager, uint32_t task_id, RTClass rt_class,
                        int64_t period_us, int64_t deadline_us, int64_t worst_case_us);
int rt_delete_task(RealtimeManager *manager, uint32_t task_id);
int rt_task_start(RealtimeManager *manager, uint32_t task_id);
int rt_task_complete(RealtimeManager *manager, uint32_t task_id);
int rt_task_update_runtime(RealtimeManager *manager, uint32_t task_id, int64_t runtime_us);

int rt_check_deadline(RealtimeManager *manager, uint32_t task_id);
int rt_admit_task(RealtimeManager *manager, uint32_t task_id);
double rt_get_cpu_utilization(RealtimeManager *manager);

uint32_t rt_create_timer(RealtimeManager *manager, int64_t interval_us,
                         bool periodic, void (*callback)(uint32_t));
int rt_start_timer(RealtimeManager *manager, uint32_t timer_id);
int rt_stop_timer(RealtimeManager *manager, uint32_t timer_id);
int rt_delete_timer(RealtimeManager *manager, uint32_t timer_id);

RTTask *rt_get_task(RealtimeManager *manager, uint32_t task_id);
uint32_t rt_get_missed_deadlines(RealtimeManager *manager);
uint64_t rt_get_total_preemptions(RealtimeManager *manager);

#endif
