#ifndef MALI_DRIVER_H
#define MALI_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t core_mask;
    uint32_t clock_speed;
} mali_device_t;

typedef struct
{
    uint32_t core_id;
    uint32_t status;
    uint32_t cycles;
} mali_core_info_t;

int mali_init(void);
int mali_probe(void);
int mali_open(mali_device_t *dev);
int mali_close(mali_device_t *dev);
int mali_get_core_info(mali_device_t *dev, mali_core_info_t *info);
int mali_set_frequency(mali_device_t *dev, uint32_t freq);

#endif
