#include "libsystemd.h"

int libsystemd_init(void)
{
    return 0;
}

int libsystemd_bus_open_system(sd_bus **bus)
{
    if (!bus)
    {
        return -1;
    }
    return 0;
}

int libsystemd_bus_close(sd_bus *bus)
{
    if (!bus)
    {
        return -1;
    }
    return 0;
}

int libsystemd_event_new(sd_event **event)
{
    if (!event)
    {
        return -1;
    }
    return 0;
}

int libsystemd_event_unref(sd_event *event)
{
    if (!event)
    {
        return -1;
    }
    return 0;
}

int libsystemd_bus_call_method(sd_bus *bus, const char *destination, const char *path, const char *interface, const char *member)
{
    if (!bus || !destination || !path || !interface || !member)
    {
        return -1;
    }
    return 0;
}
