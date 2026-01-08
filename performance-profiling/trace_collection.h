#ifndef TRACE_COLLECTION_H
#define TRACE_COLLECTION_H

#include <stdint.h>
#include <stddef.h>

#define MAX_TRACE_EVENTS 524288
#define MAX_TRACE_BUFFERS 256
#define MAX_TRACE_FILTERS 64

typedef enum
{
    TRACE_EVENT_FUNCTION_ENTRY,
    TRACE_EVENT_FUNCTION_EXIT,
    TRACE_EVENT_SYSCALL_ENTRY,
    TRACE_EVENT_SYSCALL_EXIT,
    TRACE_EVENT_IRQ_ENTRY,
    TRACE_EVENT_IRQ_EXIT,
    TRACE_EVENT_CONTEXT_SWITCH,
    TRACE_EVENT_CUSTOM
} TraceEventType;

typedef struct
{
    uint64_t timestamp_ns;
    TraceEventType event_type;
    uint32_t process_id;
    uint32_t thread_id;
    uint32_t cpu_id;

    uint64_t event_data1;
    uint64_t event_data2;
    uint32_t stack_depth;
} TraceEvent;

typedef struct
{
    uint32_t filter_id;
    TraceEventType event_type;
    uint8_t is_enabled;
} TraceFilter;

typedef struct
{
    uint32_t buffer_id;
    TraceEvent *events;
    uint32_t event_capacity;
    uint32_t event_count;

    uint64_t total_bytes;
    uint8_t is_overflow;
} TraceBuffer;

typedef struct
{
    uint32_t collector_id;

    TraceBuffer buffers[MAX_TRACE_BUFFERS];
    uint32_t buffer_count;

    TraceFilter filters[MAX_TRACE_FILTERS];
    uint32_t filter_count;

    uint64_t total_events_collected;
    uint64_t total_events_lost;

    uint8_t is_recording;
    uint8_t is_paused;
} TraceCollector;

TraceCollector *trace_collector_init(uint32_t buffer_count);
int trace_collector_destroy(TraceCollector *collector);

int trace_create_buffer(TraceCollector *collector, uint32_t capacity);
int trace_destroy_buffer(TraceCollector *collector, uint32_t buffer_id);

int trace_record_event(TraceCollector *collector, TraceEventType type, uint32_t pid,
                       uint32_t tid, uint32_t cpu, uint64_t data1, uint64_t data2);

int trace_add_filter(TraceCollector *collector, TraceEventType event_type);
int trace_remove_filter(TraceCollector *collector, TraceEventType event_type);

int trace_start_recording(TraceCollector *collector);
int trace_stop_recording(TraceCollector *collector);
int trace_pause_recording(TraceCollector *collector);
int trace_resume_recording(TraceCollector *collector);

int trace_clear_buffers(TraceCollector *collector);
int trace_read_events(TraceCollector *collector, uint32_t buffer_id, TraceEvent *events,
                      uint32_t max_events);

int trace_save_to_file(TraceCollector *collector, const char *filename);

typedef struct
{
    uint32_t collector_id;
    uint64_t total_events;
    uint64_t events_lost;
    uint32_t buffer_count;
    float collection_efficiency;
} TraceMetrics;

int trace_get_metrics(TraceCollector *collector, TraceMetrics *metrics);

#endif
