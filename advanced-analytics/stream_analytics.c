#include "stream_analytics.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

StreamAnalyticsEngine *stream_init(void)
{
    StreamAnalyticsEngine *engine = (StreamAnalyticsEngine *)malloc(sizeof(StreamAnalyticsEngine));
    if (!engine)
        return NULL;

    memset(engine, 0, sizeof(StreamAnalyticsEngine));
    engine->processor_count = 0;
    engine->window_count = 0;
    engine->operator_count = 0;
    engine->state_count = 0;

    return engine;
}

void stream_cleanup(StreamAnalyticsEngine *engine)
{
    if (!engine)
        return;

    for (uint32_t i = 0; i < engine->window_count; i++)
    {
        if (engine->windows[i].elements)
            free(engine->windows[i].elements);
    }

    for (uint32_t i = 0; i < engine->state_count; i++)
    {
        if (engine->states[i].state_data)
            free(engine->states[i].state_data);
    }

    memset(engine, 0, sizeof(StreamAnalyticsEngine));
    free(engine);
}

uint32_t stream_create_processor(StreamAnalyticsEngine *engine, const char *name, uint32_t parallelism)
{
    if (!engine || !name || engine->processor_count >= MAX_STREAM_PROCESSORS)
        return 0;

    StreamProcessor *processor = &engine->processors[engine->processor_count];
    processor->processor_id = engine->processor_count + 1;
    strncpy(processor->processor_name, name, 127);
    processor->stream_count = 0;
    processor->window_count = 0;
    processor->operator_count = 0;
    processor->total_events = 0;
    processor->total_events_processed = 0;
    processor->throughput_events_sec = 0.0f;
    processor->parallelism = parallelism;
    processor->active = true;
    processor->created_at = time(NULL);

    engine->processor_count++;
    return processor->processor_id;
}

int stream_delete_processor(StreamAnalyticsEngine *engine, uint32_t processor_id)
{
    if (!engine || processor_id == 0 || processor_id > engine->processor_count)
        return -1;

    StreamProcessor *processor = &engine->processors[processor_id - 1];
    processor->active = false;

    return 0;
}

uint32_t stream_create_window(StreamAnalyticsEngine *engine, uint32_t processor_id, WindowType type, uint64_t size_ms, uint64_t advance_ms)
{
    if (!engine || processor_id == 0 || processor_id > engine->processor_count || engine->window_count >= MAX_STREAM_WINDOWS)
        return 0;

    StreamWindow *window = &engine->windows[engine->window_count];
    window->window_id = engine->window_count + 1;
    window->type = type;
    window->window_size_ms = size_ms;
    window->window_advance_ms = advance_ms;
    window->window_start_time = 0;
    window->window_end_time = 0;
    window->element_count = 0;
    window->max_elements = 10000;
    window->elements = (uint32_t *)malloc(sizeof(uint32_t) * window->max_elements);
    window->is_active = true;
    window->watermark = 0.0f;

    engine->window_count++;
    engine->processors[processor_id - 1].window_count++;

    return window->window_id;
}

int stream_trigger_window(StreamAnalyticsEngine *engine, uint32_t window_id)
{
    if (!engine || window_id == 0 || window_id > engine->window_count)
        return -1;

    StreamWindow *window = &engine->windows[window_id - 1];
    time_t now = time(NULL);
    window->window_start_time = now * 1000;
    window->window_end_time = window->window_start_time + window->window_size_ms;

    return 0;
}

int stream_close_window(StreamAnalyticsEngine *engine, uint32_t window_id)
{
    if (!engine || window_id == 0 || window_id > engine->window_count)
        return -1;

    StreamWindow *window = &engine->windows[window_id - 1];
    window->is_active = false;

    return 0;
}

uint32_t stream_add_operator(StreamAnalyticsEngine *engine, uint32_t processor_id, OperatorType type, const char *name)
{
    if (!engine || processor_id == 0 || processor_id > engine->processor_count || !name || engine->operator_count >= MAX_STREAM_OPERATORS)
        return 0;

    StreamOperator *operator = &engine->operators[engine->operator_count];
    operator->operator_id = engine->operator_count + 1;
    operator->stream_id = processor_id;
    operator->type = type;
    strncpy(operator->operator_name, name, 127);
    operator->input_count = 0;
    operator->output_count = 0;
    operator->error_count = 0;
    operator->enabled = true;
    operator->created_at = time(NULL);

    engine->operator_count++;
    engine->processors[processor_id - 1].operator_count++;

    return operator->operator_id;
}

int stream_apply_operator(StreamAnalyticsEngine *engine, uint32_t operator_id, const uint8_t *input, uint32_t input_size, uint8_t *output, uint32_t *output_size)
{
    if (!engine || operator_id == 0 || operator_id > engine->operator_count || !input || input_size == 0 || !output || !output_size)
        return -1;

    StreamOperator *stream_operator = &engine->operators[operator_id - 1];
    stream_operator->input_count++;

    switch (stream_operator->type)
    {
    case OPERATOR_TYPE_MAP:
        memcpy(output, input, input_size);
        *output_size = input_size;
        break;
    case OPERATOR_TYPE_FILTER:
        if (input_size > 0)
        {
            memcpy(output, input, input_size);
            *output_size = input_size;
        }
        break;
    case OPERATOR_TYPE_AGGREGATE:
        *output_size = sizeof(uint64_t);
        *(uint64_t *)output = (uint64_t)input_size;
        break;
    default:
        *output_size = 0;
        stream_operator->error_count++;
        return -2;
    }

    stream_operator->output_count++;
    return 0;
}

int stream_put_state(StreamAnalyticsEngine *engine, const char *state_key, const uint8_t *data, uint32_t size)
{
    if (!engine || !state_key || !data || size == 0 || engine->state_count >= MAX_STREAM_STATES)
        return -1;

    StreamState *state = &engine->states[engine->state_count];
    state->state_id = engine->state_count + 1;
    strncpy(state->state_key, state_key, 127);
    state->state_version = 1;
    state->state_data = (uint8_t *)malloc(size);
    memcpy(state->state_data, data, size);
    state->state_size = size;
    state->created_at = time(NULL);
    state->updated_at = state->created_at;
    state->access_count = 0;

    engine->state_count++;

    return 0;
}

int stream_get_state(StreamAnalyticsEngine *engine, const char *state_key, uint8_t *data, uint32_t *size)
{
    if (!engine || !state_key || !data || !size)
        return -1;

    for (uint32_t i = 0; i < engine->state_count; i++)
    {
        if (strcmp(engine->states[i].state_key, state_key) == 0)
        {
            if (*size < engine->states[i].state_size)
                return -2;

            memcpy(data, engine->states[i].state_data, engine->states[i].state_size);
            *size = engine->states[i].state_size;
            engine->states[i].access_count++;
            engine->states[i].updated_at = time(NULL);

            return 0;
        }
    }

    return -3;
}

int stream_delete_state(StreamAnalyticsEngine *engine, const char *state_key)
{
    if (!engine || !state_key)
        return -1;

    for (uint32_t i = 0; i < engine->state_count; i++)
    {
        if (strcmp(engine->states[i].state_key, state_key) == 0)
        {
            if (engine->states[i].state_data)
                free(engine->states[i].state_data);

            if (i < engine->state_count - 1)
            {
                memcpy(&engine->states[i], &engine->states[engine->state_count - 1], sizeof(StreamState));
            }

            engine->state_count--;
            return 0;
        }
    }

    return -2;
}

int stream_get_processor_stats(StreamAnalyticsEngine *engine, uint32_t processor_id, uint64_t *events_processed, float *throughput)
{
    if (!engine || processor_id == 0 || processor_id > engine->processor_count || !events_processed || !throughput)
        return -1;

    StreamProcessor *processor = &engine->processors[processor_id - 1];
    *events_processed = processor->total_events_processed;
    *throughput = processor->throughput_events_sec;

    return 0;
}

int stream_checkpoint_state(StreamAnalyticsEngine *engine, uint32_t processor_id)
{
    if (!engine || processor_id == 0 || processor_id > engine->processor_count)
        return -1;

    for (uint32_t i = 0; i < engine->state_count; i++)
    {
        engine->states[i].updated_at = time(NULL);
    }

    return 0;
}
