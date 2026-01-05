#include "kernel_perf.h"

int kernel_perf_init(void)
{
    return 0;
}

int kernel_perf_register_event(kernel_perf_event_t *event)
{
    if (!event)
    {
        return -1;
    }
    return 0;
}

int kernel_perf_unregister_event(uint32_t event_id)
{
    return 0;
}

int kernel_perf_start_sampling(uint32_t event_id)
{
    return 0;
}

int kernel_perf_stop_sampling(uint32_t event_id)
{
    return 0;
}

int kernel_perf_get_event_data(uint32_t event_id, uint64_t *data)
{
    if (!data)
    {
        return -1;
    }
    return 0;
}
