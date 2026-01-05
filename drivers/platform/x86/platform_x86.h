#ifndef PLATFORM_X86_H
#define PLATFORM_X86_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t subsys_id;
    uint32_t flags;
    uint32_t base_address;
} platform_x86_device_t;

typedef struct
{
    uint32_t acpi_handle;
    uint32_t irq_number;
    uint32_t resources;
} platform_x86_resource_t;

int platform_x86_init(void);
int platform_x86_probe(platform_x86_device_t *dev);
int platform_x86_enable(platform_x86_device_t *dev);
int platform_x86_disable(platform_x86_device_t *dev);
int platform_x86_get_resources(platform_x86_device_t *dev, platform_x86_resource_t *res);
int platform_x86_set_config(platform_x86_device_t *dev, uint32_t *config, uint32_t size);

#endif
