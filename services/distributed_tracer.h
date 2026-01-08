#ifndef DISTRIBUTED_TRACER_H
#define DISTRIBUTED_TRACER_H

#include <stdint.h>
#include <stddef.h>
#include <time.h>

#define MAX_TRACES 1024
#define MAX_SPANS 8192
#define MAX_SPAN_TAGS 32
#define MAX_SERVICES 128
#define MAX_TRACE_DEPTH 256

typedef enum
{
    SPAN_CLIENT = 0,
    SPAN_SERVER = 1,
    SPAN_PRODUCER = 2,
    SPAN_CONSUMER = 3,
    SPAN_INTERNAL = 4
} SpanKind;

typedef enum
{
    SPAN_PENDING = 0,
    SPAN_RUNNING = 1,
    SPAN_FINISHED = 2,
    SPAN_ERROR = 3
} SpanStatus;

typedef struct
{
    char key[64];
    char value[256];
} SpanTag;

typedef struct
{
    uint64_t span_id;
    uint64_t trace_id;
    uint64_t parent_span_id;
    char operation_name[128];
    SpanKind kind;
    SpanStatus status;
    uint64_t start_time_us;
    uint64_t end_time_us;
    uint32_t tag_count;
    SpanTag tags[MAX_SPAN_TAGS];
    char service_name[64];
    uint32_t error_code;
    char error_message[256];
} Span;

typedef struct
{
    uint64_t trace_id;
    char trace_name[128];
    uint64_t start_time_us;
    uint64_t end_time_us;
    uint32_t span_count;
    uint64_t span_ids[MAX_SPANS];
    uint32_t service_count;
    char services[MAX_SERVICES][64];
    uint64_t total_duration_us;
    SpanStatus status;
    uint8_t is_sampled;
} Trace;

typedef struct
{
    uint32_t collector_id;
    char name[64];
    uint32_t trace_count;
    Trace traces[MAX_TRACES];
    uint32_t span_count;
    Span spans[MAX_SPANS];
    uint64_t total_traces_collected;
    uint64_t total_spans_collected;
    uint32_t sampling_rate;
    uint64_t bytes_collected;
} TraceCollector;

int trace_create_collector(const char *name);
int trace_delete_collector(uint32_t collector_id);

uint64_t trace_start_trace(uint32_t collector_id, const char *trace_name);
int trace_end_trace(uint32_t collector_id, uint64_t trace_id);

uint64_t trace_start_span(uint32_t collector_id, uint64_t trace_id, uint64_t parent_span_id,
                          const char *operation_name, const char *service_name, SpanKind kind);
int trace_end_span(uint32_t collector_id, uint64_t span_id);
int trace_set_span_status(uint32_t collector_id, uint64_t span_id, SpanStatus status);

int trace_add_span_tag(uint32_t collector_id, uint64_t span_id, const char *key, const char *value);
int trace_add_span_error(uint32_t collector_id, uint64_t span_id, uint32_t error_code, const char *message);

int trace_set_sampling_rate(uint32_t collector_id, uint32_t rate_percent);
int trace_should_sample(uint32_t collector_id);

Trace *trace_get_trace(uint32_t collector_id, uint64_t trace_id);
Span *trace_get_span(uint32_t collector_id, uint64_t span_id);
uint64_t trace_get_trace_duration(uint32_t collector_id, uint64_t trace_id);
int trace_get_critical_path(uint32_t collector_id, uint64_t trace_id, uint64_t *path_spans, uint32_t *path_count);

TraceCollector *trace_get_collector_info(uint32_t collector_id);
uint32_t trace_get_error_span_count(uint32_t collector_id);
uint64_t trace_get_total_span_latency(uint32_t collector_id);
int trace_export_traces(uint32_t collector_id, const char *format);

#endif 
