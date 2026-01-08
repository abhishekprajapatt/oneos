#include "perf_monitor.h"
#include <stdlib.h>
#include <string.h>

PerfMonitor *perf_init(void)
{
    PerfMonitor *monitor = (PerfMonitor *)malloc(sizeof(PerfMonitor));
    if (!monitor)
        return NULL;

    memset(monitor, 0, sizeof(PerfMonitor));
    monitor->counter_count = 0;
    monitor->metric_count = 0;
    monitor->trace_count = 0;
    monitor->profile_count = 0;
    monitor->current_trace_level = TRACE_LEVEL_WARNING;
    monitor->profiling_enabled = false;
    monitor->tracing_enabled = true;
    monitor->total_samples = 0;
    monitor->avg_cpu_usage = 0.0;
    monitor->avg_memory_usage = 0.0;

    return monitor;
}

void perf_cleanup(PerfMonitor *monitor)
{
    if (!monitor)
        return;
    memset(monitor, 0, sizeof(PerfMonitor));
    free(monitor);
}

uint32_t perf_create_counter(PerfMonitor *monitor, CounterType type)
{
    if (!monitor || monitor->counter_count >= MAX_COUNTERS)
        return 0;

    Counter *counter = &monitor->counters[monitor->counter_count];
    counter->counter_id = monitor->counter_count + 1;
    counter->type = type;
    counter->value = 0;
    counter->delta = 0;
    counter->last_reset = time(NULL);
    counter->sample_count = 0;

    monitor->counter_count++;
    return counter->counter_id;
}

int perf_increment_counter(PerfMonitor *monitor, uint32_t counter_id, uint64_t value)
{
    if (!monitor)
        return -1;

    Counter *counter = perf_get_counter(monitor, counter_id);
    if (!counter)
        return -1;

    counter->value += value;
    counter->delta += value;
    counter->sample_count++;
    monitor->total_samples++;

    return 0;
}

int perf_reset_counter(PerfMonitor *monitor, uint32_t counter_id)
{
    if (!monitor)
        return -1;

    Counter *counter = perf_get_counter(monitor, counter_id);
    if (!counter)
        return -1;

    counter->delta = 0;
    counter->last_reset = time(NULL);
    return 0;
}

Counter *perf_get_counter(PerfMonitor *monitor, uint32_t counter_id)
{
    if (!monitor)
        return NULL;

    for (uint32_t i = 0; i < monitor->counter_count; i++)
    {
        if (monitor->counters[i].counter_id == counter_id)
        {
            return &monitor->counters[i];
        }
    }
    return NULL;
}

uint32_t perf_create_metric(PerfMonitor *monitor, MetricType type)
{
    if (!monitor || monitor->metric_count >= MAX_METRICS)
        return 0;

    Metric *metric = &monitor->metrics[monitor->metric_count];
    metric->metric_id = monitor->metric_count + 1;
    metric->type = type;
    metric->current_value = 0.0;
    metric->min_value = 0.0;
    metric->max_value = 0.0;
    metric->average_value = 0.0;
    metric->samples_collected = 0;
    metric->last_updated = time(NULL);

    monitor->metric_count++;
    return metric->metric_id;
}

int perf_record_metric(PerfMonitor *monitor, uint32_t metric_id, double value)
{
    if (!monitor)
        return -1;

    Metric *metric = perf_get_metric(monitor, metric_id);
    if (!metric)
        return -1;

    metric->current_value = value;
    metric->samples_collected++;

    if (metric->samples_collected == 1)
    {
        metric->min_value = value;
        metric->max_value = value;
        metric->average_value = value;
    }
    else
    {
        if (value < metric->min_value)
            metric->min_value = value;
        if (value > metric->max_value)
            metric->max_value = value;
        metric->average_value =
            (metric->average_value * (metric->samples_collected - 1) + value) / metric->samples_collected;
    }

    metric->last_updated = time(NULL);
    return 0;
}

Metric *perf_get_metric(PerfMonitor *monitor, uint32_t metric_id)
{
    if (!monitor)
        return NULL;

    for (uint32_t i = 0; i < monitor->metric_count; i++)
    {
        if (monitor->metrics[i].metric_id == metric_id)
        {
            return &monitor->metrics[i];
        }
    }
    return NULL;
}

int perf_set_trace_level(PerfMonitor *monitor, TraceLevel level)
{
    if (!monitor)
        return -1;
    monitor->current_trace_level = level;
    return 0;
}

TraceLevel perf_get_trace_level(PerfMonitor *monitor)
{
    if (!monitor)
        return TRACE_LEVEL_NONE;
    return monitor->current_trace_level;
}

int perf_record_trace(PerfMonitor *monitor, TraceLevel level, uint32_t process_id,
                      uint32_t thread_id, const char *function, const char *message, uint32_t duration_us)
{
    if (!monitor || monitor->trace_count >= MAX_TRACES)
        return -1;
    if (level > monitor->current_trace_level)
        return 0;

    TraceRecord *trace = &monitor->traces[monitor->trace_count];
    trace->trace_id = monitor->trace_count + 1;
    trace->level = level;
    trace->timestamp = time(NULL);
    trace->process_id = process_id;
    trace->thread_id = thread_id;
    trace->duration_us = duration_us;

    if (function)
    {
        strncpy(trace->function_name, function, 127);
        trace->function_name[127] = '\0';
    }

    if (message)
    {
        strncpy(trace->message, message, 511);
        trace->message[511] = '\0';
    }

    monitor->trace_count++;
    return 0;
}

TraceRecord *perf_get_trace(PerfMonitor *monitor, uint32_t trace_id)
{
    if (!monitor)
        return NULL;

    for (uint32_t i = 0; i < monitor->trace_count; i++)
    {
        if (monitor->traces[i].trace_id == trace_id)
        {
            return &monitor->traces[i];
        }
    }
    return NULL;
}

int perf_enable_profiling(PerfMonitor *monitor, bool enable)
{
    if (!monitor)
        return -1;
    monitor->profiling_enabled = enable;
    return 0;
}

uint32_t perf_start_profile(PerfMonitor *monitor, uint32_t process_id)
{
    if (!monitor || monitor->profile_count >= MAX_PROFILING_DATA)
        return 0;

    ProfileData *profile = &monitor->profiles[monitor->profile_count];
    profile->profile_id = monitor->profile_count + 1;
    profile->process_id = process_id;
    profile->samples = 0;
    profile->cpu_cycles = 0;
    profile->instructions = 0;
    profile->ipc = 0.0f;
    profile->cache_hit_ratio = 0.0f;
    profile->start_time = time(NULL);
    profile->end_time = 0;

    monitor->profile_count++;
    return profile->profile_id;
}

int perf_end_profile(PerfMonitor *monitor, uint32_t profile_id)
{
    if (!monitor)
        return -1;

    ProfileData *profile = perf_get_profile(monitor, profile_id);
    if (!profile)
        return -1;

    profile->end_time = time(NULL);
    if (profile->instructions > 0)
    {
        profile->ipc = (float)profile->cpu_cycles / profile->instructions;
    }

    return 0;
}

ProfileData *perf_get_profile(PerfMonitor *monitor, uint32_t profile_id)
{
    if (!monitor)
        return NULL;

    for (uint32_t i = 0; i < monitor->profile_count; i++)
    {
        if (monitor->profiles[i].profile_id == profile_id)
        {
            return &monitor->profiles[i];
        }
    }
    return NULL;
}

uint32_t perf_get_total_counter_count(PerfMonitor *monitor)
{
    if (!monitor)
        return 0;
    return monitor->counter_count;
}

uint32_t perf_get_total_metric_count(PerfMonitor *monitor)
{
    if (!monitor)
        return 0;
    return monitor->metric_count;
}

uint32_t perf_get_total_trace_count(PerfMonitor *monitor)
{
    if (!monitor)
        return 0;
    return monitor->trace_count;
}

double perf_get_average_cpu_usage(PerfMonitor *monitor)
{
    if (!monitor)
        return 0.0;

    double total = 0.0;
    for (uint32_t i = 0; i < monitor->metric_count; i++)
    {
        if (monitor->metrics[i].type == METRIC_CPU_USAGE)
        {
            total += monitor->metrics[i].average_value;
        }
    }

    monitor->avg_cpu_usage = total / (monitor->metric_count + 1);
    return monitor->avg_cpu_usage;
}

double perf_get_average_memory_usage(PerfMonitor *monitor)
{
    if (!monitor)
        return 0.0;

    double total = 0.0;
    for (uint32_t i = 0; i < monitor->metric_count; i++)
    {
        if (monitor->metrics[i].type == METRIC_MEMORY_USAGE)
        {
            total += monitor->metrics[i].average_value;
        }
    }

    monitor->avg_memory_usage = total / (monitor->metric_count + 1);
    return monitor->avg_memory_usage;
}

int perf_generate_report(PerfMonitor *monitor, const char *filename)
{
    if (!monitor || !filename)
        return -1;
    return 0;
}

int perf_collect_all_metrics(PerfMonitor *monitor)
{
    if (!monitor)
        return -1;

    perf_get_average_cpu_usage(monitor);
    perf_get_average_memory_usage(monitor);

    return monitor->metric_count;
}
