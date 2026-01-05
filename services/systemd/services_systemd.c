#include "services_systemd.h"

int services_systemd_init(void)
{
    return 0;
}

int services_systemd_load_unit(services_systemd_unit_t *unit)
{
    if (!unit)
    {
        return -1;
    }
    return 0;
}

int services_systemd_start_unit(uint32_t unit_id)
{
    return 0;
}

int services_systemd_stop_unit(uint32_t unit_id)
{
    return 0;
}

int services_systemd_restart_unit(uint32_t unit_id)
{
    return 0;
}

int services_systemd_enable_unit(uint32_t unit_id)
{
    return 0;
}

int services_systemd_disable_unit(uint32_t unit_id)
{
    return 0;
}

int services_systemd_get_unit_status(uint32_t unit_id, services_systemd_unit_t *unit)
{
    if (!unit)
    {
        return -1;
    }
    return 0;
}

int services_systemd_get_state(services_systemd_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
