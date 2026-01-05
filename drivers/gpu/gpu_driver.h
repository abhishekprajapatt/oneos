#ifndef GPU_DRIVER_H
#define GPU_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t vram_size;
    uint32_t clock_speed;
} gpu_device_t;

typedef struct
{
    uint32_t vertex_count;
    uint32_t index_count;
    void *vertex_data;
    void *index_data;
} gpu_command_t;

int gpu_init(void);
int gpu_open(gpu_device_t *dev);
int gpu_close(gpu_device_t *dev);
int gpu_submit_command(gpu_device_t *dev, gpu_command_t *cmd);
int gpu_wait_idle(gpu_device_t *dev);

#endif
