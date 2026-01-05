#include "i915_driver.h"

int i915_init(void)
{
    return 0;
}

int i915_probe(void)
{
    return 0;
}

int i915_open(i915_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int i915_close(i915_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int i915_get_pipe(i915_device_t *dev, uint32_t pipe, i915_pipe_t *pipe_info)
{
    if (!dev || !pipe_info)
    {
        return -1;
    }
    return 0;
}

int i915_set_mode(i915_device_t *dev, i915_pipe_t *pipe)
{
    if (!dev || !pipe)
    {
        return -1;
    }
    return 0;
}
