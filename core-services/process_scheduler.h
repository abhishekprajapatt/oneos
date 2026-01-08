#ifndef PROCESS_SCHEDULER_H
#define PROCESS_SCHEDULER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_SCHEDULED_PROCESSES 512

typedef enum
{
    PRIORITY_REALTIME = 0,
    PRIORITY_HIGH = 1,
    PRIORITY_NORMAL = 2,
    PRIORITY_LOW = 3,
    PRIORITY_IDLE = 4
} ProcessPriority;

typedef enum
{
    PROC_STATE_NEW,
    PROC_STATE_READY,
    PROC_STATE_RUNNING,
    PROC_STATE_WAITING,
    PROC_STATE_TERMINATED
} ProcessState;

typedef struct
{
    uint32_t pid;
    char name[256];
    ProcessState state;
    ProcessPriority priority;
    uint32_t cpu_time;
    uint32_t memory_usage;
    uint32_t context_switches;
    uint64_t creation_time;
} ScheduledProcess;

typedef struct
{
    ScheduledProcess processes[MAX_SCHEDULED_PROCESSES];
    uint32_t process_count;
    uint32_t current_process_idx;
    uint32_t time_slice_ms;
} ProcessScheduler;

ProcessScheduler *scheduler_create(void);
void scheduler_destroy(ProcessScheduler *scheduler);
uint32_t scheduler_add_process(ProcessScheduler *scheduler, const char *name, ProcessPriority priority);
void scheduler_remove_process(ProcessScheduler *scheduler, uint32_t pid);
void scheduler_schedule_next(ProcessScheduler *scheduler);
ScheduledProcess *scheduler_get_current_process(ProcessScheduler *scheduler);
void scheduler_update_process_state(ProcessScheduler *scheduler, uint32_t pid, ProcessState state);

#endif
