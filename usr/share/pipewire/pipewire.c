#include "pipewire.h"

int pipewire_init(void)
{
    return 0;
}

int pipewire_list_devices(pipewire_device_t *devices, uint32_t *count)
{
    if (!devices || !count)
    {
        return -1;
    }
    return 0;
}

int pipewire_get_device(uint32_t device_id, pipewire_device_t *device)
{
    if (device_id == 0 || !device)
    {
        return -1;
    }
    return 0;
}

int pipewire_set_default_device(uint32_t device_id)
{
    if (device_id == 0)
    {
        return -1;
    }
    return 0;
}

int pipewire_get_device_properties(uint32_t device_id, char *properties, uint32_t max_size)
{
    if (device_id == 0 || !properties || max_size == 0)
    {
        return -1;
    }
    return 0;
}

int pipewire_connect_stream(uint32_t source_id, uint32_t sink_id)
{
    if (source_id == 0 || sink_id == 0)
    {
        return -1;
    }
    return 0;
}

int pipewire_get_stats(pipewire_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
