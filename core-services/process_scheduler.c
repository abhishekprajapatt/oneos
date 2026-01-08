#include "process_scheduler.h"
#include <stdlib.h>
#include <string.h>
#include <time.h>

ProcessScheduler *scheduler_create(void)
{
    ProcessScheduler *scheduler = (ProcessScheduler *)malloc(sizeof(ProcessScheduler));
    if (!scheduler)
        return NULL;

    scheduler->process_count = 0;
    scheduler->current_process_idx = 0;
    scheduler->time_slice_ms = 10;

    memset(scheduler->processes, 0, sizeof(scheduler->processes));

    return scheduler;
}

void scheduler_destroy(ProcessScheduler *scheduler)
{
    if (!scheduler)
        return;
    free(scheduler);
}

uint32_t scheduler_add_process(ProcessScheduler *scheduler, const char *name, ProcessPriority priority)
{
    if (!scheduler || scheduler->process_count >= MAX_SCHEDULED_PROCESSES || !name)
        return 0;

    ScheduledProcess *proc = &scheduler->processes[scheduler->process_count];
    proc->pid = scheduler->process_count + 1;
    strcpy(proc->name, name);
    proc->state = PROC_STATE_NEW;
    proc->priority = priority;
    proc->cpu_time = 0;
    proc->memory_usage = 0;
    proc->context_switches = 0;
    proc->creation_time = time(NULL);

    scheduler->process_count++;
    return proc->pid;
}

void scheduler_remove_process(ProcessScheduler *scheduler, uint32_t pid)
{
    if (!scheduler)
        return;

    for (uint32_t i = 0; i < scheduler->process_count; i++)
    {
        if (scheduler->processes[i].pid == pid)
        {
            for (uint32_t j = i; j < scheduler->process_count - 1; j++)
            {
                scheduler->processes[j] = scheduler->processes[j + 1];
            }
            scheduler->process_count--;
            break;
        }
    }
}

void scheduler_schedule_next(ProcessScheduler *scheduler)
{
    if (!scheduler || scheduler->process_count == 0)
        return;

    uint32_t next_idx = (scheduler->current_process_idx + 1) % scheduler->process_count;
    scheduler->current_process_idx = next_idx;
}

ScheduledProcess *scheduler_get_current_process(ProcessScheduler *scheduler)
{
    if (!scheduler || scheduler->process_count == 0)
        return NULL;
    return &scheduler->processes[scheduler->current_process_idx];
}

void scheduler_update_process_state(ProcessScheduler *scheduler, uint32_t pid, ProcessState state)
{
    if (!scheduler)
        return;

    for (uint32_t i = 0; i < scheduler->process_count; i++)
    {
        if (scheduler->processes[i].pid == pid)
        {
            scheduler->processes[i].state = state;
            break;
        }
    }
}
