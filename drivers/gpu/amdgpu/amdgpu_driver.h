#ifndef AMDGPU_DRIVER_H
#define AMDGPU_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t family;
    uint32_t vram_size;
    uint32_t clock_speed;
} amdgpu_device_t;

typedef struct
{
    uint32_t handle;
    uint32_t offset;
    uint32_t size;
} amdgpu_buffer_t;

int amdgpu_init(void);
int amdgpu_probe(void);
int amdgpu_open(amdgpu_device_t *dev);
int amdgpu_close(amdgpu_device_t *dev);
int amdgpu_create_buffer(amdgpu_device_t *dev, uint32_t size, amdgpu_buffer_t *buf);
int amdgpu_destroy_buffer(amdgpu_device_t *dev, amdgpu_buffer_t *buf);
int amdgpu_submit_job(amdgpu_device_t *dev, void *job);

#endif
