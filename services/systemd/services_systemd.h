#ifndef SERVICES_SYSTEMD_H
#define SERVICES_SYSTEMD_H

#include <stdint.h>

typedef struct
{
    uint32_t unit_id;
    const char *unit_name;
    uint32_t unit_type;
    uint32_t unit_state;
    uint32_t enabled;
} services_systemd_unit_t;

typedef struct
{
    uint32_t total_units;
    uint32_t active_units;
    uint32_t inactive_units;
    uint32_t failed_units;
} services_systemd_state_t;

int services_systemd_init(void);
int services_systemd_load_unit(services_systemd_unit_t *unit);
int services_systemd_start_unit(uint32_t unit_id);
int services_systemd_stop_unit(uint32_t unit_id);
int services_systemd_restart_unit(uint32_t unit_id);
int services_systemd_enable_unit(uint32_t unit_id);
int services_systemd_disable_unit(uint32_t unit_id);
int services_systemd_get_unit_status(uint32_t unit_id, services_systemd_unit_t *unit);
int services_systemd_get_state(services_systemd_state_t *state);

#endif
