#include "etc_systemd.h"

int etc_systemd_init(void)
{
    return 0;
}

int etc_systemd_load_unit(const char *unit_name, etc_systemd_unit_t *unit)
{
    if (!unit_name || !unit)
    {
        return -1;
    }
    return 0;
}

int etc_systemd_start_unit(const char *unit_name)
{
    if (!unit_name)
    {
        return -1;
    }
    return 0;
}

int etc_systemd_stop_unit(const char *unit_name)
{
    if (!unit_name)
    {
        return -1;
    }
    return 0;
}

int etc_systemd_restart_unit(const char *unit_name)
{
    if (!unit_name)
    {
        return -1;
    }
    return 0;
}

int etc_systemd_enable_unit(const char *unit_name)
{
    if (!unit_name)
    {
        return -1;
    }
    return 0;
}

int etc_systemd_disable_unit(const char *unit_name)
{
    if (!unit_name)
    {
        return -1;
    }
    return 0;
}

int etc_systemd_get_unit_status(const char *unit_name)
{
    if (!unit_name)
    {
        return -1;
    }
    return 0;
}
