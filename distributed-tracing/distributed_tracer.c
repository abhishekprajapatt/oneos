#include "distributed_tracer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static DistributedTracer *global_distributed_tracer = NULL;
static uint64_t next_trace_id = 1;
static uint64_t next_span_id = 1;

DistributedTracer *distributed_tracer_init(void)
{
    DistributedTracer *tracer = (DistributedTracer *)malloc(sizeof(DistributedTracer));
    if (!tracer)
        return NULL;

    memset(tracer, 0, sizeof(DistributedTracer));
    tracer->tracer_id = 1;
    tracer->trace_count = 0;
    tracer->span_count = 0;
    tracer->total_traces_created = 0;
    tracer->total_spans_created = 0;
    tracer->active_traces = 0;
    tracer->error_traces = 0;
    tracer->sampling_rate = 1000000;
    tracer->auto_instrument_enabled = 0;

    global_distributed_tracer = tracer;
    return tracer;
}

int distributed_tracer_destroy(DistributedTracer *tracer)
{
    if (!tracer)
        return -1;

    free(tracer);
    global_distributed_tracer = NULL;
    return 0;
}

uint64_t trace_create(DistributedTracer *tracer, const char *trace_name, const char *service_name)
{
    if (!tracer || tracer->trace_count >= MAX_TRACES || !trace_name || !service_name)
        return 0;

    Trace *trace = &tracer->traces[tracer->trace_count];
    memset(trace, 0, sizeof(Trace));

    trace->trace_id = next_trace_id++;
    strncpy(trace->trace_name, trace_name, sizeof(trace->trace_name) - 1);
    strncpy(trace->service_name, service_name, sizeof(trace->service_name) - 1);
    trace->span_count = 0;
    trace->trace_start_time_nanoseconds = 0;
    trace->trace_end_time_nanoseconds = 0;
    trace->trace_duration_milliseconds = 0;
    trace->is_complete = 0;
    trace->has_errors = 0;

    tracer->trace_count++;
    tracer->total_traces_created++;
    tracer->active_traces++;

    return trace->trace_id;
}

int trace_destroy(DistributedTracer *tracer, uint64_t trace_id)
{
    if (!tracer || trace_id == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->trace_count; i++)
    {
        if (tracer->traces[i].trace_id == trace_id)
        {
            memmove(&tracer->traces[i], &tracer->traces[i + 1],
                    sizeof(Trace) * (tracer->trace_count - i - 1));
            tracer->trace_count--;
            return 0;
        }
    }

    return -1;
}

uint64_t span_create_root(DistributedTracer *tracer, uint64_t trace_id, const char *span_name,
                          SpanKind span_kind)
{
    if (!tracer || trace_id == 0 || !span_name || tracer->span_count >= 4096)
        return 0;

    for (uint32_t i = 0; i < tracer->trace_count; i++)
    {
        if (tracer->traces[i].trace_id == trace_id)
        {
            Trace *trace = &tracer->traces[i];
            if (trace->span_count >= 64)
                return 0;

            Span *span = &tracer->spans[tracer->span_count];
            memset(span, 0, sizeof(Span));

            span->trace_id = trace_id;
            span->span_id = next_span_id++;
            span->parent_span_id = 0;
            strncpy(span->span_name, span_name, sizeof(span->span_name) - 1);
            span->span_kind = span_kind;
            span->span_status = SPAN_STATUS_UNSET;
            span->start_time_nanoseconds = 0;
            span->end_time_nanoseconds = 0;
            span->duration_milliseconds = 0;
            span->tag_count = 0;
            span->log_count = 0;
            span->is_sampled = 0;
            span->is_finished = 0;

            trace->span_ids[trace->span_count] = span->span_id;
            tracer->span_count++;
            trace->span_count++;
            tracer->total_spans_created++;

            return span->span_id;
        }
    }

    return 0;
}

uint64_t span_create_child(DistributedTracer *tracer, uint64_t trace_id, uint64_t parent_span_id,
                           const char *span_name, SpanKind span_kind)
{
    if (!tracer || trace_id == 0 || parent_span_id == 0 || !span_name || tracer->span_count >= 4096)
        return 0;

    for (uint32_t i = 0; i < tracer->trace_count; i++)
    {
        if (tracer->traces[i].trace_id == trace_id)
        {
            Trace *trace = &tracer->traces[i];
            if (trace->span_count >= 64)
                return 0;

            Span *span = &tracer->spans[tracer->span_count];
            memset(span, 0, sizeof(Span));

            span->trace_id = trace_id;
            span->span_id = next_span_id++;
            span->parent_span_id = parent_span_id;
            strncpy(span->span_name, span_name, sizeof(span->span_name) - 1);
            span->span_kind = span_kind;
            span->span_status = SPAN_STATUS_UNSET;
            span->start_time_nanoseconds = 0;
            span->end_time_nanoseconds = 0;
            span->duration_milliseconds = 0;
            span->tag_count = 0;
            span->log_count = 0;
            span->is_sampled = 0;
            span->is_finished = 0;

            trace->span_ids[trace->span_count] = span->span_id;
            tracer->span_count++;
            trace->span_count++;
            tracer->total_spans_created++;

            return span->span_id;
        }
    }

    return 0;
}

int span_start(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id)
{
    if (!tracer || trace_id == 0 || span_id == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->span_count; i++)
    {
        if (tracer->spans[i].trace_id == trace_id && tracer->spans[i].span_id == span_id)
        {
            tracer->spans[i].start_time_nanoseconds = 0;
            return 0;
        }
    }

    return -1;
}

int span_finish(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id)
{
    if (!tracer || trace_id == 0 || span_id == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->span_count; i++)
    {
        if (tracer->spans[i].trace_id == trace_id && tracer->spans[i].span_id == span_id)
        {
            tracer->spans[i].end_time_nanoseconds = 0;
            tracer->spans[i].is_finished = 1;
            return 0;
        }
    }

    return -1;
}

int span_set_status(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id,
                    SpanStatus status)
{
    if (!tracer || trace_id == 0 || span_id == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->span_count; i++)
    {
        if (tracer->spans[i].trace_id == trace_id && tracer->spans[i].span_id == span_id)
        {
            tracer->spans[i].span_status = status;
            if (status == SPAN_STATUS_ERROR)
            {
                for (uint32_t j = 0; j < tracer->trace_count; j++)
                {
                    if (tracer->traces[j].trace_id == trace_id)
                    {
                        tracer->traces[j].has_errors = 1;
                        tracer->error_traces++;
                        break;
                    }
                }
            }
            return 0;
        }
    }

    return -1;
}

int span_add_tag(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id,
                 const char *tag_key, const char *tag_value)
{
    if (!tracer || trace_id == 0 || span_id == 0 || !tag_key || !tag_value)
        return -1;

    for (uint32_t i = 0; i < tracer->span_count; i++)
    {
        if (tracer->spans[i].trace_id == trace_id && tracer->spans[i].span_id == span_id)
        {
            Span *span = &tracer->spans[i];
            if (span->tag_count >= MAX_TRACE_TAGS)
                return -1;

            snprintf(span->tags[span->tag_count], sizeof(span->tags[0]), "%s=%s", tag_key, tag_value);
            span->tag_count++;
            return 0;
        }
    }

    return -1;
}

int span_add_log(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id,
                 const char *log_message)
{
    if (!tracer || trace_id == 0 || span_id == 0 || !log_message)
        return -1;

    for (uint32_t i = 0; i < tracer->span_count; i++)
    {
        if (tracer->spans[i].trace_id == trace_id && tracer->spans[i].span_id == span_id)
        {
            Span *span = &tracer->spans[i];
            if (span->log_count >= MAX_TRACE_LOGS)
                return -1;

            strncpy(span->logs[span->log_count], log_message, sizeof(span->logs[0]) - 1);
            span->log_count++;
            return 0;
        }
    }

    return -1;
}

int span_set_sampled(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id)
{
    if (!tracer || trace_id == 0 || span_id == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->span_count; i++)
    {
        if (tracer->spans[i].trace_id == trace_id && tracer->spans[i].span_id == span_id)
        {
            tracer->spans[i].is_sampled = 1;
            return 0;
        }
    }

    return -1;
}

int trace_complete(DistributedTracer *tracer, uint64_t trace_id)
{
    if (!tracer || trace_id == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->trace_count; i++)
    {
        if (tracer->traces[i].trace_id == trace_id)
        {
            Trace *trace = &tracer->traces[i];
            trace->is_complete = 1;
            trace->trace_end_time_nanoseconds = 0;
            tracer->active_traces--;
            return 0;
        }
    }

    return -1;
}

int trace_mark_error(DistributedTracer *tracer, uint64_t trace_id)
{
    if (!tracer || trace_id == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->trace_count; i++)
    {
        if (tracer->traces[i].trace_id == trace_id)
        {
            tracer->traces[i].has_errors = 1;
            return 0;
        }
    }

    return -1;
}

int distributed_tracer_set_sampling_rate(DistributedTracer *tracer, uint64_t rate)
{
    if (!tracer)
        return -1;

    tracer->sampling_rate = rate;
    return 0;
}

int distributed_tracer_enable_auto_instrumentation(DistributedTracer *tracer)
{
    if (!tracer)
        return -1;

    tracer->auto_instrument_enabled = 1;
    return 0;
}

int distributed_tracer_export_trace(DistributedTracer *tracer, uint64_t trace_id,
                                    char *buffer, uint32_t buffer_size)
{
    if (!tracer || trace_id == 0 || !buffer || buffer_size == 0)
        return -1;

    for (uint32_t i = 0; i < tracer->trace_count; i++)
    {
        if (tracer->traces[i].trace_id == trace_id)
        {
            Trace *trace = &tracer->traces[i];
            snprintf(buffer, buffer_size, "Trace{id=%llu,name=%s,service=%s,spans=%u,errors=%u}",
                     trace->trace_id, trace->trace_name, trace->service_name, trace->span_count, trace->has_errors);
            return 0;
        }
    }

    return -1;
}

int distributed_tracer_get_metrics(DistributedTracer *tracer, DistributedTracerMetrics *metrics)
{
    if (!tracer || !metrics)
        return -1;

    metrics->tracer_id = tracer->tracer_id;
    metrics->total_traces = tracer->trace_count;
    metrics->active_traces = tracer->active_traces;
    metrics->completed_traces = tracer->trace_count - tracer->active_traces;
    metrics->error_traces = tracer->error_traces;
    metrics->total_spans = tracer->total_spans_created;
    metrics->total_traces_created = tracer->total_traces_created;
    metrics->sampling_rate = (uint32_t)(tracer->sampling_rate / 10000);

    return 0;
}
