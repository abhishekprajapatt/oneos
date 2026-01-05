#include "tools_perf.h"

int tools_perf_init(void)
{
    return 0;
}

int tools_perf_create_event(tools_perf_event_t *event)
{
    if (!event)
    {
        return -1;
    }
    return 0;
}

int tools_perf_start_event(uint32_t event_id)
{
    return 0;
}

int tools_perf_stop_event(uint32_t event_id)
{
    return 0;
}

int tools_perf_read_samples(uint32_t event_id, void *buffer, uint32_t *count)
{
    if (!buffer || !count)
    {
        return -1;
    }
    return 0;
}

int tools_perf_get_state(tools_perf_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
