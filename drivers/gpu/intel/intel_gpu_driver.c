#include "intel_gpu_driver.h"

int intel_gpu_init(void)
{
    return 0;
}

int intel_gpu_probe(void)
{
    return 0;
}

int intel_gpu_open(intel_gpu_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int intel_gpu_close(intel_gpu_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int intel_gpu_get_eu_info(intel_gpu_device_t *dev, intel_eu_info_t *info)
{
    if (!dev || !info)
    {
        return -1;
    }
    return 0;
}

int intel_gpu_execute_batch(intel_gpu_device_t *dev, void *batch)
{
    if (!dev || !batch)
    {
        return -1;
    }
    return 0;
}
