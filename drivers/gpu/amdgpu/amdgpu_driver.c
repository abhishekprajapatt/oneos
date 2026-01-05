#include "amdgpu_driver.h"

int amdgpu_init(void)
{
    return 0;
}

int amdgpu_probe(void)
{
    return 0;
}

int amdgpu_open(amdgpu_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int amdgpu_close(amdgpu_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int amdgpu_create_buffer(amdgpu_device_t *dev, uint32_t size, amdgpu_buffer_t *buf)
{
    if (!dev || !buf || size == 0)
    {
        return -1;
    }
    return 0;
}

int amdgpu_destroy_buffer(amdgpu_device_t *dev, amdgpu_buffer_t *buf)
{
    if (!dev || !buf)
    {
        return -1;
    }
    return 0;
}

int amdgpu_submit_job(amdgpu_device_t *dev, void *job)
{
    if (!dev || !job)
    {
        return -1;
    }
    return 0;
}
