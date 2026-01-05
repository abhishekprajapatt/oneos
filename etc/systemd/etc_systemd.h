#ifndef ETC_SYSTEMD_H
#define ETC_SYSTEMD_H

#include <stdint.h>

typedef struct
{
    uint32_t unit_id;
    const char *unit_name;
    const char *unit_type;
    uint32_t status;
} etc_systemd_unit_t;

typedef struct
{
    uint32_t unit_count;
    uint32_t active_units;
    uint32_t failed_units;
} etc_systemd_state_t;

int etc_systemd_init(void);
int etc_systemd_load_unit(const char *unit_name, etc_systemd_unit_t *unit);
int etc_systemd_start_unit(const char *unit_name);
int etc_systemd_stop_unit(const char *unit_name);
int etc_systemd_restart_unit(const char *unit_name);
int etc_systemd_enable_unit(const char *unit_name);
int etc_systemd_disable_unit(const char *unit_name);
int etc_systemd_get_unit_status(const char *unit_name);

#endif
