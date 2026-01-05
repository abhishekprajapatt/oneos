#ifndef TOOLS_PERF_H
#define TOOLS_PERF_H

#include <stdint.h>

typedef struct
{
    uint32_t event_id;
    const char *event_name;
    const char *event_type;
    uint64_t sample_count;
    uint64_t overflow_count;
} tools_perf_event_t;

typedef struct
{
    uint32_t active_events;
    uint64_t total_samples;
    uint64_t total_overflows;
    uint32_t cpu_count;
} tools_perf_state_t;

int tools_perf_init(void);
int tools_perf_create_event(tools_perf_event_t *event);
int tools_perf_start_event(uint32_t event_id);
int tools_perf_stop_event(uint32_t event_id);
int tools_perf_read_samples(uint32_t event_id, void *buffer, uint32_t *count);
int tools_perf_get_state(tools_perf_state_t *state);

#endif
