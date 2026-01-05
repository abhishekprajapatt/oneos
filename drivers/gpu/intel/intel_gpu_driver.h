#ifndef INTEL_GPU_DRIVER_H
#define INTEL_GPU_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t pci_id;
    uint32_t eu_count;
} intel_gpu_device_t;

typedef struct
{
    uint32_t slice;
    uint32_t subslice;
    uint32_t eu;
} intel_eu_info_t;

int intel_gpu_init(void);
int intel_gpu_probe(void);
int intel_gpu_open(intel_gpu_device_t *dev);
int intel_gpu_close(intel_gpu_device_t *dev);
int intel_gpu_get_eu_info(intel_gpu_device_t *dev, intel_eu_info_t *info);
int intel_gpu_execute_batch(intel_gpu_device_t *dev, void *batch);

#endif
