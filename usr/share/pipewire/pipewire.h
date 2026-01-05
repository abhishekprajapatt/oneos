#ifndef PIPEWIRE_H
#define PIPEWIRE_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    const char *device_name;
    const char *device_type;
    uint32_t channels;
    uint32_t sample_rate;
} pipewire_device_t;

typedef struct
{
    uint32_t active_devices;
    uint32_t total_devices;
    uint32_t running_streams;
    uint64_t total_latency;
} pipewire_stats_t;

int pipewire_init(void);
int pipewire_list_devices(pipewire_device_t *devices, uint32_t *count);
int pipewire_get_device(uint32_t device_id, pipewire_device_t *device);
int pipewire_set_default_device(uint32_t device_id);
int pipewire_get_device_properties(uint32_t device_id, char *properties, uint32_t max_size);
int pipewire_connect_stream(uint32_t source_id, uint32_t sink_id);
int pipewire_get_stats(pipewire_stats_t *stats);

#endif
