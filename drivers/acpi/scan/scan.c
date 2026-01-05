#include "scan.h"

int acpi_scan_init(void)
{
    return 0;
}

int acpi_scan_start(void)
{
    return 0;
}

int acpi_scan_devices(acpi_scan_device_t *devices, uint32_t *count)
{
    if (!devices || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int acpi_scan_get_device(uint32_t id, acpi_scan_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
