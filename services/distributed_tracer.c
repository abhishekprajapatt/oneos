#include "distributed_tracer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static TraceCollector g_collectors[16] = {0};
static uint32_t g_collector_count = 0;
static uint64_t g_trace_counter = 1;
static uint64_t g_span_counter = 1;

int trace_create_collector(const char *name)
{
    if (!name || g_collector_count >= 16)
    {
        return -1;
    }

    TraceCollector *collector = &g_collectors[g_collector_count];
    collector->collector_id = g_collector_count + 1;
    strncpy(collector->name, name, 63);
    collector->trace_count = 0;
    collector->span_count = 0;
    collector->total_traces_collected = 0;
    collector->total_spans_collected = 0;
    collector->sampling_rate = 100;
    collector->bytes_collected = 0;

    g_collector_count++;
    return collector->collector_id;
}

int trace_delete_collector(uint32_t collector_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return -1;
    }

    for (uint32_t i = collector_id - 1; i < g_collector_count - 1; i++)
    {
        g_collectors[i] = g_collectors[i + 1];
    }
    g_collector_count--;

    return 0;
}

uint64_t trace_start_trace(uint32_t collector_id, const char *trace_name)
{
    if (collector_id == 0 || collector_id > g_collector_count || !trace_name)
    {
        return 0;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];
    if (collector->trace_count >= MAX_TRACES)
    {
        return 0;
    }

    Trace *trace = &collector->traces[collector->trace_count];
    trace->trace_id = g_trace_counter++;
    strncpy(trace->trace_name, trace_name, 127);
    trace->start_time_us = time(NULL) * 1000000;
    trace->end_time_us = 0;
    trace->span_count = 0;
    trace->service_count = 0;
    trace->total_duration_us = 0;
    trace->status = SPAN_PENDING;
    trace->is_sampled = (rand() % 100) < collector->sampling_rate ? 1 : 0;

    collector->trace_count++;
    collector->total_traces_collected++;

    return trace->trace_id;
}

int trace_end_trace(uint32_t collector_id, uint64_t trace_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return -1;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];

    for (uint32_t i = 0; i < collector->trace_count; i++)
    {
        if (collector->traces[i].trace_id == trace_id)
        {
            collector->traces[i].end_time_us = time(NULL) * 1000000;
            collector->traces[i].total_duration_us =
                collector->traces[i].end_time_us - collector->traces[i].start_time_us;
            collector->traces[i].status = SPAN_FINISHED;
            return 0;
        }
    }

    return -1;
}

uint64_t trace_start_span(uint32_t collector_id, uint64_t trace_id, uint64_t parent_span_id,
                          const char *operation_name, const char *service_name, SpanKind kind)
{
    if (collector_id == 0 || collector_id > g_collector_count || !operation_name || !service_name)
    {
        return 0;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];
    if (collector->span_count >= MAX_SPANS)
    {
        return 0;
    }

    Span *span = &collector->spans[collector->span_count];
    span->span_id = g_span_counter++;
    span->trace_id = trace_id;
    span->parent_span_id = parent_span_id;
    strncpy(span->operation_name, operation_name, 127);
    strncpy(span->service_name, service_name, 63);
    span->kind = kind;
    span->status = SPAN_RUNNING;
    span->start_time_us = time(NULL) * 1000000;
    span->end_time_us = 0;
    span->tag_count = 0;
    span->error_code = 0;
    memset(span->error_message, 0, 256);

    collector->span_count++;
    collector->total_spans_collected++;
    collector->bytes_collected += sizeof(Span);

    for (uint32_t i = 0; i < collector->trace_count; i++)
    {
        if (collector->traces[i].trace_id == trace_id)
        {
            if (collector->traces[i].span_count < MAX_SPANS)
            {
                collector->traces[i].span_ids[collector->traces[i].span_count++] = span->span_id;

                int found = 0;
                for (uint32_t j = 0; j < collector->traces[i].service_count; j++)
                {
                    if (strcmp(collector->traces[i].services[j], service_name) == 0)
                    {
                        found = 1;
                        break;
                    }
                }

                if (!found && collector->traces[i].service_count < MAX_SERVICES)
                {
                    strncpy(collector->traces[i].services[collector->traces[i].service_count++],
                            service_name, 63);
                }
            }
            break;
        }
    }

    return span->span_id;
}

int trace_end_span(uint32_t collector_id, uint64_t span_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return -1;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];

    for (uint32_t i = 0; i < collector->span_count; i++)
    {
        if (collector->spans[i].span_id == span_id)
        {
            collector->spans[i].end_time_us = time(NULL) * 1000000;
            collector->spans[i].status = SPAN_FINISHED;
            return 0;
        }
    }

    return -1;
}

int trace_set_span_status(uint32_t collector_id, uint64_t span_id, SpanStatus status)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return -1;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];

    for (uint32_t i = 0; i < collector->span_count; i++)
    {
        if (collector->spans[i].span_id == span_id)
        {
            collector->spans[i].status = status;
            return 0;
        }
    }

    return -1;
}

int trace_add_span_tag(uint32_t collector_id, uint64_t span_id, const char *key, const char *value)
{
    if (collector_id == 0 || collector_id > g_collector_count || !key || !value)
    {
        return -1;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];

    for (uint32_t i = 0; i < collector->span_count; i++)
    {
        if (collector->spans[i].span_id == span_id)
        {
            if (collector->spans[i].tag_count >= MAX_SPAN_TAGS)
            {
                return -1;
            }

            SpanTag *tag = &collector->spans[i].tags[collector->spans[i].tag_count++];
            strncpy(tag->key, key, 63);
            strncpy(tag->value, value, 255);
            return 0;
        }
    }

    return -1;
}

int trace_add_span_error(uint32_t collector_id, uint64_t span_id, uint32_t error_code, const char *message)
{
    if (collector_id == 0 || collector_id > g_collector_count || !message)
    {
        return -1;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];

    for (uint32_t i = 0; i < collector->span_count; i++)
    {
        if (collector->spans[i].span_id == span_id)
        {
            collector->spans[i].error_code = error_code;
            strncpy(collector->spans[i].error_message, message, 255);
            collector->spans[i].status = SPAN_ERROR;
            return 0;
        }
    }

    return -1;
}

int trace_set_sampling_rate(uint32_t collector_id, uint32_t rate_percent)
{
    if (collector_id == 0 || collector_id > g_collector_count || rate_percent > 100)
    {
        return -1;
    }

    g_collectors[collector_id - 1].sampling_rate = rate_percent;
    return 0;
}

int trace_should_sample(uint32_t collector_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return 0;
    }

    return (rand() % 100) < g_collectors[collector_id - 1].sampling_rate;
}

Trace *trace_get_trace(uint32_t collector_id, uint64_t trace_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return NULL;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];
    for (uint32_t i = 0; i < collector->trace_count; i++)
    {
        if (collector->traces[i].trace_id == trace_id)
        {
            return &collector->traces[i];
        }
    }

    return NULL;
}

Span *trace_get_span(uint32_t collector_id, uint64_t span_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return NULL;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];
    for (uint32_t i = 0; i < collector->span_count; i++)
    {
        if (collector->spans[i].span_id == span_id)
        {
            return &collector->spans[i];
        }
    }

    return NULL;
}

uint64_t trace_get_trace_duration(uint32_t collector_id, uint64_t trace_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return 0;
    }

    Trace *trace = trace_get_trace(collector_id, trace_id);
    if (!trace)
    {
        return 0;
    }

    return trace->total_duration_us;
}

int trace_get_critical_path(uint32_t collector_id, uint64_t trace_id, uint64_t *path_spans, uint32_t *path_count)
{
    if (collector_id == 0 || collector_id > g_collector_count || !path_spans || !path_count)
    {
        return -1;
    }

    Trace *trace = trace_get_trace(collector_id, trace_id);
    if (!trace)
    {
        return -1;
    }

    uint32_t count = 0;
    for (uint32_t i = 0; i < trace->span_count && count < *path_count; i++)
    {
        path_spans[count++] = trace->span_ids[i];
    }
    *path_count = count;

    return 0;
}

TraceCollector *trace_get_collector_info(uint32_t collector_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return NULL;
    }

    return &g_collectors[collector_id - 1];
}

uint32_t trace_get_error_span_count(uint32_t collector_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return 0;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];
    uint32_t error_count = 0;

    for (uint32_t i = 0; i < collector->span_count; i++)
    {
        if (collector->spans[i].status == SPAN_ERROR)
        {
            error_count++;
        }
    }

    return error_count;
}

uint64_t trace_get_total_span_latency(uint32_t collector_id)
{
    if (collector_id == 0 || collector_id > g_collector_count)
    {
        return 0;
    }

    TraceCollector *collector = &g_collectors[collector_id - 1];
    uint64_t total_latency = 0;

    for (uint32_t i = 0; i < collector->span_count; i++)
    {
        if (collector->spans[i].end_time_us > 0)
        {
            total_latency += (collector->spans[i].end_time_us - collector->spans[i].start_time_us);
        }
    }

    return total_latency;
}

int trace_export_traces(uint32_t collector_id, const char *format)
{
    if (collector_id == 0 || collector_id > g_collector_count || !format)
    {
        return -1;
    }

    return 0;
}
