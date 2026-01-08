#include "trace_collection.h"
#include <stdlib.h>
#include <string.h>

static TraceCollector *global_trace_collector = NULL;
static uint32_t next_buffer_id = 1;

TraceCollector *trace_collector_init(uint32_t buffer_count)
{
    TraceCollector *collector = (TraceCollector *)malloc(sizeof(TraceCollector));
    if (!collector)
        return NULL;

    memset(collector, 0, sizeof(TraceCollector));
    collector->collector_id = 1;
    collector->buffer_count = 0;
    collector->filter_count = 0;
    collector->total_events_collected = 0;
    collector->total_events_lost = 0;
    collector->is_recording = 0;
    collector->is_paused = 0;

    for (uint32_t i = 0; i < buffer_count && i < MAX_TRACE_BUFFERS; i++)
    {
        trace_create_buffer(collector, MAX_TRACE_EVENTS);
    }

    global_trace_collector = collector;
    return collector;
}

int trace_collector_destroy(TraceCollector *collector)
{
    if (!collector)
        return -1;

    for (uint32_t i = 0; i < collector->buffer_count; i++)
    {
        if (collector->buffers[i].events)
            free(collector->buffers[i].events);
    }

    free(collector);
    global_trace_collector = NULL;
    return 0;
}

int trace_create_buffer(TraceCollector *collector, uint32_t capacity)
{
    if (!collector || collector->buffer_count >= MAX_TRACE_BUFFERS || capacity == 0)
        return -1;

    TraceBuffer *buffer = &collector->buffers[collector->buffer_count];
    memset(buffer, 0, sizeof(TraceBuffer));

    buffer->buffer_id = next_buffer_id++;
    buffer->events = (TraceEvent *)malloc(sizeof(TraceEvent) * capacity);
    if (!buffer->events)
        return -1;

    buffer->event_capacity = capacity;
    buffer->event_count = 0;
    buffer->total_bytes = capacity * sizeof(TraceEvent);
    buffer->is_overflow = 0;

    collector->buffer_count++;
    return buffer->buffer_id;
}

int trace_destroy_buffer(TraceCollector *collector, uint32_t buffer_id)
{
    if (!collector || buffer_id == 0)
        return -1;

    for (uint32_t i = 0; i < collector->buffer_count; i++)
    {
        if (collector->buffers[i].buffer_id == buffer_id)
        {
            if (collector->buffers[i].events)
                free(collector->buffers[i].events);

            memmove(&collector->buffers[i], &collector->buffers[i + 1],
                    sizeof(TraceBuffer) * (collector->buffer_count - i - 1));
            collector->buffer_count--;
            return 0;
        }
    }

    return -1;
}

int trace_record_event(TraceCollector *collector, TraceEventType type, uint32_t pid,
                       uint32_t tid, uint32_t cpu, uint64_t data1, uint64_t data2)
{
    if (!collector || collector->buffer_count == 0 || !collector->is_recording)
        return -1;

    uint8_t should_record = 0;

    if (collector->filter_count == 0)
    {
        should_record = 1;
    }
    else
    {
        for (uint32_t i = 0; i < collector->filter_count; i++)
        {
            if (collector->filters[i].event_type == type && collector->filters[i].is_enabled)
            {
                should_record = 1;
                break;
            }
        }
    }

    if (!should_record)
        return 0;

    TraceBuffer *buffer = &collector->buffers[0];

    if (buffer->event_count >= buffer->event_capacity)
    {
        buffer->is_overflow = 1;
        collector->total_events_lost++;
        return -1;
    }

    TraceEvent *evt = &buffer->events[buffer->event_count];
    evt->timestamp_ns = 0;
    evt->event_type = type;
    evt->process_id = pid;
    evt->thread_id = tid;
    evt->cpu_id = cpu;
    evt->event_data1 = data1;
    evt->event_data2 = data2;
    evt->stack_depth = 0;

    buffer->event_count++;
    collector->total_events_collected++;

    return 0;
}

int trace_add_filter(TraceCollector *collector, TraceEventType event_type)
{
    if (!collector || collector->filter_count >= MAX_TRACE_FILTERS)
        return -1;

    for (uint32_t i = 0; i < collector->filter_count; i++)
    {
        if (collector->filters[i].event_type == event_type)
            return -1;
    }

    TraceFilter *filter = &collector->filters[collector->filter_count];
    filter->filter_id = collector->filter_count + 1;
    filter->event_type = event_type;
    filter->is_enabled = 1;

    collector->filter_count++;
    return filter->filter_id;
}

int trace_remove_filter(TraceCollector *collector, TraceEventType event_type)
{
    if (!collector)
        return -1;

    for (uint32_t i = 0; i < collector->filter_count; i++)
    {
        if (collector->filters[i].event_type == event_type)
        {
            memmove(&collector->filters[i], &collector->filters[i + 1],
                    sizeof(TraceFilter) * (collector->filter_count - i - 1));
            collector->filter_count--;
            return 0;
        }
    }

    return -1;
}

int trace_start_recording(TraceCollector *collector)
{
    if (!collector)
        return -1;

    collector->is_recording = 1;
    collector->is_paused = 0;
    return 0;
}

int trace_stop_recording(TraceCollector *collector)
{
    if (!collector)
        return -1;

    collector->is_recording = 0;
    collector->is_paused = 0;
    return 0;
}

int trace_pause_recording(TraceCollector *collector)
{
    if (!collector)
        return -1;

    collector->is_paused = 1;
    return 0;
}

int trace_resume_recording(TraceCollector *collector)
{
    if (!collector)
        return -1;

    collector->is_paused = 0;
    return 0;
}

int trace_clear_buffers(TraceCollector *collector)
{
    if (!collector)
        return -1;

    for (uint32_t i = 0; i < collector->buffer_count; i++)
    {
        collector->buffers[i].event_count = 0;
        collector->buffers[i].is_overflow = 0;
    }

    return 0;
}

int trace_read_events(TraceCollector *collector, uint32_t buffer_id, TraceEvent *events,
                      uint32_t max_events)
{
    if (!collector || buffer_id == 0 || !events || max_events == 0)
        return -1;

    for (uint32_t i = 0; i < collector->buffer_count; i++)
    {
        if (collector->buffers[i].buffer_id == buffer_id)
        {
            TraceBuffer *buffer = &collector->buffers[i];
            uint32_t to_copy = buffer->event_count < max_events ? buffer->event_count : max_events;
            memcpy(events, buffer->events, sizeof(TraceEvent) * to_copy);
            return to_copy;
        }
    }

    return -1;
}

int trace_save_to_file(TraceCollector *collector, const char *filename)
{
    if (!collector || !filename)
        return -1;

    return 0;
}

int trace_get_metrics(TraceCollector *collector, TraceMetrics *metrics)
{
    if (!collector || !metrics)
        return -1;

    metrics->collector_id = collector->collector_id;
    metrics->total_events = collector->total_events_collected;
    metrics->events_lost = collector->total_events_lost;
    metrics->buffer_count = collector->buffer_count;

    if (collector->total_events_collected + collector->total_events_lost > 0)
    {
        metrics->collection_efficiency = (float)collector->total_events_collected /
                                         (collector->total_events_collected + collector->total_events_lost);
    }
    else
    {
        metrics->collection_efficiency = 0.0f;
    }

    return 0;
}
