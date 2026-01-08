#include "perf_counter.h"
#include <stdlib.h>
#include <string.h>

static PerfCounterManager *global_perf_counter = NULL;

PerfCounterManager *perf_counter_init(uint32_t core_count)
{
    PerfCounterManager *manager = (PerfCounterManager *)malloc(sizeof(PerfCounterManager));
    if (!manager)
        return NULL;

    memset(manager, 0, sizeof(PerfCounterManager));
    manager->manager_id = 1;
    manager->core_count = (core_count > MAX_CPU_CORES) ? MAX_CPU_CORES : core_count;
    manager->total_cycles = 0;
    manager->total_instructions = 0;
    manager->total_cache_misses = 0;
    manager->total_page_faults = 0;
    manager->ipc = 0.0f;
    manager->cache_miss_rate = 0.0f;
    manager->context_switch_rate = 0.0f;

    for (uint32_t i = 0; i < manager->core_count; i++)
    {
        manager->cores[i].counter_id = i;
        manager->cores[i].core_id = i;
        manager->cores[i].counter_count = 0;
        manager->cores[i].enabled_time_ns = 0;
        manager->cores[i].running_time_ns = 0;
        manager->cores[i].multiplexing_ratio = 1.0f;

        for (uint32_t j = 0; j < MAX_COUNTER_EVENTS; j++)
        {
            manager->cores[i].counters[j].event_type = (PerfEventType)j;
            snprintf(manager->cores[i].counters[j].event_name, sizeof(manager->cores[i].counters[j].event_name),
                     "event_%d", j);
            manager->cores[i].counters[j].count = 0;
            manager->cores[i].counters[j].peak_count = 0;
            manager->cores[i].counters[j].min_count = UINT64_MAX;
            manager->cores[i].counters[j].max_count = 0;
            manager->cores[i].counters[j].measurement_count = 0;
            manager->cores[i].counters[j].average = 0.0f;
        }
    }

    global_perf_counter = manager;
    return manager;
}

int perf_counter_destroy(PerfCounterManager *manager)
{
    if (!manager)
        return -1;

    free(manager);
    global_perf_counter = NULL;
    return 0;
}

int perf_counter_add_event(PerfCounterManager *manager, uint32_t core_id, PerfEventType event)
{
    if (!manager || core_id >= manager->core_count || event >= MAX_COUNTER_EVENTS)
        return -1;

    CorePerfCounters *core = &manager->cores[core_id];

    for (uint32_t i = 0; i < core->counter_count; i++)
    {
        if (core->counters[i].event_type == event)
            return -1;
    }

    if (core->counter_count >= MAX_COUNTER_EVENTS)
        return -1;

    core->counters[core->counter_count].event_type = event;
    core->counter_count++;

    return 0;
}

int perf_counter_remove_event(PerfCounterManager *manager, uint32_t core_id, PerfEventType event)
{
    if (!manager || core_id >= manager->core_count || event >= MAX_COUNTER_EVENTS)
        return -1;

    CorePerfCounters *core = &manager->cores[core_id];

    for (uint32_t i = 0; i < core->counter_count; i++)
    {
        if (core->counters[i].event_type == event)
        {
            memmove(&core->counters[i], &core->counters[i + 1],
                    sizeof(PerfCounter) * (core->counter_count - i - 1));
            core->counter_count--;
            return 0;
        }
    }

    return -1;
}

int perf_counter_enable(PerfCounterManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->core_count; i++)
    {
        manager->cores[i].enabled_time_ns = 0;
    }

    return 0;
}

int perf_counter_disable(PerfCounterManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->core_count; i++)
    {
        manager->cores[i].enabled_time_ns = 0;
    }

    return 0;
}

int perf_counter_read(PerfCounterManager *manager, uint32_t core_id, PerfEventType event, uint64_t *count)
{
    if (!manager || core_id >= manager->core_count || event >= MAX_COUNTER_EVENTS || !count)
        return -1;

    CorePerfCounters *core = &manager->cores[core_id];
    PerfCounter *counter = &core->counters[event];

    *count = counter->count;
    return 0;
}

int perf_counter_reset(PerfCounterManager *manager, uint32_t core_id)
{
    if (!manager || core_id >= manager->core_count)
        return -1;

    CorePerfCounters *core = &manager->cores[core_id];

    for (uint32_t i = 0; i < core->counter_count; i++)
    {
        core->counters[i].count = 0;
    }

    return 0;
}

int perf_counter_reset_all(PerfCounterManager *manager)
{
    if (!manager)
        return -1;

    for (uint32_t i = 0; i < manager->core_count; i++)
    {
        perf_counter_reset(manager, i);
    }

    return 0;
}

int perf_counter_calculate_ipc(PerfCounterManager *manager)
{
    if (!manager)
        return -1;

    uint64_t total_instructions = 0;
    uint64_t total_cycles = 0;

    for (uint32_t i = 0; i < manager->core_count; i++)
    {
        for (uint32_t j = 0; j < manager->cores[i].counter_count; j++)
        {
            if (manager->cores[i].counters[j].event_type == PERF_EVENT_INSTRUCTIONS)
            {
                total_instructions += manager->cores[i].counters[j].count;
            }
            if (manager->cores[i].counters[j].event_type == PERF_EVENT_CPU_CYCLES)
            {
                total_cycles += manager->cores[i].counters[j].count;
            }
        }
    }

    manager->total_instructions = total_instructions;
    manager->total_cycles = total_cycles;

    if (total_cycles > 0)
    {
        manager->ipc = (float)total_instructions / total_cycles;
    }
    else
    {
        manager->ipc = 0.0f;
    }

    return 0;
}

int perf_counter_calculate_cache_miss_rate(PerfCounterManager *manager)
{
    if (!manager)
        return -1;

    uint64_t total_misses = 0;
    uint64_t total_refs = 0;

    for (uint32_t i = 0; i < manager->core_count; i++)
    {
        for (uint32_t j = 0; j < manager->cores[i].counter_count; j++)
        {
            if (manager->cores[i].counters[j].event_type == PERF_EVENT_CACHE_MISSES)
            {
                total_misses += manager->cores[i].counters[j].count;
            }
            if (manager->cores[i].counters[j].event_type == PERF_EVENT_CACHE_REFERENCES)
            {
                total_refs += manager->cores[i].counters[j].count;
            }
        }
    }

    manager->total_cache_misses = total_misses;

    if (total_refs > 0)
    {
        manager->cache_miss_rate = (float)total_misses / total_refs;
    }
    else
    {
        manager->cache_miss_rate = 0.0f;
    }

    return 0;
}

int perf_counter_get_metrics(PerfCounterManager *manager, PerfMetrics *metrics)
{
    if (!manager || !metrics)
        return -1;

    perf_counter_calculate_ipc(manager);
    perf_counter_calculate_cache_miss_rate(manager);

    metrics->manager_id = manager->manager_id;
    metrics->core_count = manager->core_count;
    metrics->total_cycles = manager->total_cycles;
    metrics->total_instructions = manager->total_instructions;
    metrics->ipc = manager->ipc;
    metrics->cache_miss_rate = manager->cache_miss_rate;

    return 0;
}
