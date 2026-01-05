#ifndef KERNEL_PERF_H
#define KERNEL_PERF_H

#include <stdint.h>

typedef struct
{
    uint32_t event_id;
    const char *event_name;
    uint32_t event_type;
    uint64_t event_count;
} kernel_perf_event_t;

typedef struct
{
    uint32_t event_count;
    uint64_t total_cycles;
    uint64_t total_instructions;
} kernel_perf_state_t;

int kernel_perf_init(void);
int kernel_perf_register_event(kernel_perf_event_t *event);
int kernel_perf_unregister_event(uint32_t event_id);
int kernel_perf_start_sampling(uint32_t event_id);
int kernel_perf_stop_sampling(uint32_t event_id);
int kernel_perf_get_event_data(uint32_t event_id, uint64_t *data);

#endif
