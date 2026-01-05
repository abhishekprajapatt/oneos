#ifndef RADEON_DRIVER_H
#define RADEON_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t family;
    uint32_t vram_size;
    uint32_t vram_type;
} radeon_device_t;

typedef struct
{
    uint32_t handle;
    uint32_t offset;
    uint32_t size;
    uint32_t domain;
} radeon_buffer_t;

int radeon_init(void);
int radeon_probe(void);
int radeon_open(radeon_device_t *dev);
int radeon_close(radeon_device_t *dev);
int radeon_buffer_create(radeon_device_t *dev, uint32_t size, radeon_buffer_t *buf);
int radeon_buffer_destroy(radeon_device_t *dev, radeon_buffer_t *buf);
int radeon_cs_submit(radeon_device_t *dev, void *cs);

#endif
