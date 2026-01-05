#ifndef ACPI_CORE_H
#define ACPI_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t address;
    uint32_t size;
} acpi_memory_t;

typedef struct
{
    uint8_t type;
    uint8_t reserved;
    uint16_t length;
} acpi_header_t;

typedef struct
{
    uint32_t id;
    uint32_t flags;
    uint32_t power_state;
} acpi_device_t;

int acpi_init(void);
int acpi_shutdown(void);
int acpi_get_device(uint32_t id, acpi_device_t *dev);
int acpi_set_power_state(uint32_t id, uint32_t state);

#endif
