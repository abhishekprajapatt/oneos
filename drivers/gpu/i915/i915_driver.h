#ifndef I915_DRIVER_H
#define I915_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t gen;
    uint32_t num_pipes;
    uint32_t vram_size;
} i915_device_t;

typedef struct
{
    uint32_t pipe;
    uint32_t enabled;
    uint32_t width;
    uint32_t height;
    uint32_t refresh_rate;
} i915_pipe_t;

int i915_init(void);
int i915_probe(void);
int i915_open(i915_device_t *dev);
int i915_close(i915_device_t *dev);
int i915_get_pipe(i915_device_t *dev, uint32_t pipe, i915_pipe_t *pipe_info);
int i915_set_mode(i915_device_t *dev, i915_pipe_t *pipe);

#endif
