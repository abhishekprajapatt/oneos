#ifndef DISTRIBUTED_TRACER_H
#define DISTRIBUTED_TRACER_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TRACES 512
#define MAX_SPANS_PER_TRACE 128
#define MAX_TRACE_TAGS 32
#define MAX_TRACE_LOGS 256

typedef enum
{
    SPAN_KIND_UNSPECIFIED,
    SPAN_KIND_INTERNAL,
    SPAN_KIND_SERVER,
    SPAN_KIND_CLIENT,
    SPAN_KIND_PRODUCER,
    SPAN_KIND_CONSUMER
} SpanKind;

typedef enum
{
    SPAN_STATUS_UNSET,
    SPAN_STATUS_OK,
    SPAN_STATUS_ERROR
} SpanStatus;

typedef struct
{
    uint64_t trace_id;
    uint64_t span_id;
    uint64_t parent_span_id;

    char span_name[256];
    SpanKind span_kind;
    SpanStatus span_status;

    uint64_t start_time_nanoseconds;
    uint64_t end_time_nanoseconds;
    uint32_t duration_milliseconds;

    char tags[MAX_TRACE_TAGS][128];
    uint32_t tag_count;

    char logs[MAX_TRACE_LOGS][256];
    uint32_t log_count;

    uint8_t is_sampled;
    uint8_t is_finished;
} Span;

typedef struct
{
    uint64_t trace_id;
    char trace_name[128];

    uint32_t span_ids[64];
    uint32_t span_count;

    uint64_t trace_start_time_nanoseconds;
    uint64_t trace_end_time_nanoseconds;
    uint32_t trace_duration_milliseconds;

    char service_name[64];
    uint8_t is_complete;
    uint8_t has_errors;
} Trace;

typedef struct
{
    uint32_t tracer_id;

    Trace traces[MAX_TRACES];
    uint32_t trace_count;

    Span spans[4096];
    uint32_t span_count;

    uint64_t total_traces_created;
    uint64_t total_spans_created;
    uint32_t active_traces;
    uint32_t error_traces;

    uint64_t sampling_rate;
    uint8_t auto_instrument_enabled;
} DistributedTracer;

DistributedTracer *distributed_tracer_init(void);
int distributed_tracer_destroy(DistributedTracer *tracer);

uint64_t trace_create(DistributedTracer *tracer, const char *trace_name, const char *service_name);
int trace_destroy(DistributedTracer *tracer, uint64_t trace_id);

uint64_t span_create_root(DistributedTracer *tracer, uint64_t trace_id, const char *span_name,
                          SpanKind span_kind);

uint64_t span_create_child(DistributedTracer *tracer, uint64_t trace_id, uint64_t parent_span_id,
                           const char *span_name, SpanKind span_kind);

int span_start(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id);
int span_finish(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id);

int span_set_status(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id,
                    SpanStatus status);

int span_add_tag(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id,
                 const char *tag_key, const char *tag_value);

int span_add_log(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id,
                 const char *log_message);

int span_set_sampled(DistributedTracer *tracer, uint64_t trace_id, uint64_t span_id);

int trace_complete(DistributedTracer *tracer, uint64_t trace_id);

int trace_mark_error(DistributedTracer *tracer, uint64_t trace_id);

int distributed_tracer_set_sampling_rate(DistributedTracer *tracer, uint64_t rate);

int distributed_tracer_enable_auto_instrumentation(DistributedTracer *tracer);

int distributed_tracer_export_trace(DistributedTracer *tracer, uint64_t trace_id,
                                    char *buffer, uint32_t buffer_size);

typedef struct
{
    uint32_t tracer_id;
    uint32_t total_traces;
    uint32_t active_traces;
    uint32_t completed_traces;
    uint32_t error_traces;
    uint64_t total_spans;
    uint64_t total_traces_created;
    uint32_t sampling_rate;
} DistributedTracerMetrics;

int distributed_tracer_get_metrics(DistributedTracer *tracer, DistributedTracerMetrics *metrics);

#endif
