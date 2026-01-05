#include "acpi_core.h"

static acpi_device_t acpi_devices[256];
static int device_count = 0;

int acpi_init(void)
{
    return 0;
}

int acpi_shutdown(void)
{
    device_count = 0;
    return 0;
}

int acpi_get_device(uint32_t id, acpi_device_t *dev)
{
    if (id >= device_count || !dev)
    {
        return -1;
    }
    *dev = acpi_devices[id];
    return 0;
}

int acpi_set_power_state(uint32_t id, uint32_t state)
{
    if (id >= device_count)
    {
        return -1;
    }
    acpi_devices[id].power_state = state;
    return 0;
}
