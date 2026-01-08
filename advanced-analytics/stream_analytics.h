#ifndef STREAM_ANALYTICS_H
#define STREAM_ANALYTICS_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define MAX_STREAM_PROCESSORS 32
#define MAX_STREAM_WINDOWS 128
#define MAX_STREAM_OPERATORS 256
#define MAX_STREAM_STATES 512
#define WINDOW_BUFFER_SIZE 131072

typedef enum
{
    WINDOW_TYPE_TUMBLING,
    WINDOW_TYPE_SLIDING,
    WINDOW_TYPE_SESSION,
    WINDOW_TYPE_GLOBAL
} WindowType;

typedef enum
{
    OPERATOR_TYPE_MAP,
    OPERATOR_TYPE_FILTER,
    OPERATOR_TYPE_REDUCE,
    OPERATOR_TYPE_JOIN,
    OPERATOR_TYPE_AGGREGATE,
    OPERATOR_TYPE_FLATMAP,
    OPERATOR_TYPE_DISTINCT,
    OPERATOR_TYPE_SORT
} OperatorType;

typedef struct
{
    uint32_t window_id;
    WindowType type;
    uint64_t window_size_ms;
    uint64_t window_advance_ms;
    uint64_t window_start_time;
    uint64_t window_end_time;
    uint32_t element_count;
    uint32_t *elements;
    uint32_t max_elements;
    bool is_active;
    float watermark;
} StreamWindow;

typedef struct
{
    uint32_t operator_id;
    uint32_t stream_id;
    OperatorType type;
    char operator_name[128];
    uint64_t input_count;
    uint64_t output_count;
    uint64_t error_count;
    bool enabled;
    time_t created_at;
} StreamOperator;

typedef struct
{
    uint32_t state_id;
    char state_key[128];
    uint32_t state_version;
    uint8_t *state_data;
    uint32_t state_size;
    time_t created_at;
    time_t updated_at;
    uint64_t access_count;
} StreamState;

typedef struct
{
    uint32_t processor_id;
    char processor_name[128];
    uint32_t stream_count;
    uint32_t window_count;
    uint32_t operator_count;
    uint64_t total_events;
    uint64_t total_events_processed;
    float throughput_events_sec;
    uint32_t parallelism;
    bool active;
    time_t created_at;
} StreamProcessor;

typedef struct
{
    StreamProcessor processors[MAX_STREAM_PROCESSORS];
    StreamWindow windows[MAX_STREAM_WINDOWS];
    StreamOperator operators[MAX_STREAM_OPERATORS];
    StreamState states[MAX_STREAM_STATES];
    uint32_t processor_count;
    uint32_t window_count;
    uint32_t operator_count;
    uint32_t state_count;
} StreamAnalyticsEngine;

StreamAnalyticsEngine *stream_init(void);
void stream_cleanup(StreamAnalyticsEngine *engine);

uint32_t stream_create_processor(StreamAnalyticsEngine *engine, const char *name, uint32_t parallelism);
int stream_delete_processor(StreamAnalyticsEngine *engine, uint32_t processor_id);

uint32_t stream_create_window(StreamAnalyticsEngine *engine, uint32_t processor_id, WindowType type, uint64_t size_ms, uint64_t advance_ms);
int stream_trigger_window(StreamAnalyticsEngine *engine, uint32_t window_id);
int stream_close_window(StreamAnalyticsEngine *engine, uint32_t window_id);

uint32_t stream_add_operator(StreamAnalyticsEngine *engine, uint32_t processor_id, OperatorType type, const char *name);
int stream_apply_operator(StreamAnalyticsEngine *engine, uint32_t operator_id, const uint8_t *input, uint32_t input_size, uint8_t *output, uint32_t *output_size);

int stream_put_state(StreamAnalyticsEngine *engine, const char *state_key, const uint8_t *data, uint32_t size);
int stream_get_state(StreamAnalyticsEngine *engine, const char *state_key, uint8_t *data, uint32_t *size);
int stream_delete_state(StreamAnalyticsEngine *engine, const char *state_key);

int stream_get_processor_stats(StreamAnalyticsEngine *engine, uint32_t processor_id, uint64_t *events_processed, float *throughput);
int stream_checkpoint_state(StreamAnalyticsEngine *engine, uint32_t processor_id);

#endif
