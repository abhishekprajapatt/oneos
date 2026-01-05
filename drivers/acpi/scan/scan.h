#ifndef ACPI_SCAN_H
#define ACPI_SCAN_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t type;
    char name[256];
    uint32_t flags;
} acpi_scan_device_t;

int acpi_scan_init(void);
int acpi_scan_start(void);
int acpi_scan_devices(acpi_scan_device_t *devices, uint32_t *count);
int acpi_scan_get_device(uint32_t id, acpi_scan_device_t *dev);

#endif
