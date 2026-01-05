#ifndef VAR_LIB_SYSTEMD_H
#define VAR_LIB_SYSTEMD_H

#include <stdint.h>

typedef struct
{
    uint32_t systemd_id;
    const char *unit_name;
    const char *unit_type;
    const char *unit_status;
    uint64_t unit_size;
} var_lib_systemd_entry_t;

typedef struct
{
    uint32_t total_units;
    uint32_t active_units;
    uint32_t inactive_units;
    uint64_t total_size;
} var_lib_systemd_state_t;

int var_lib_systemd_init(void);
int var_lib_systemd_list(var_lib_systemd_entry_t *units, uint32_t *count);
int var_lib_systemd_get_unit(const char *unit_name, var_lib_systemd_entry_t *unit);
int var_lib_systemd_update_unit_state(const char *unit_name, const char *status);
int var_lib_systemd_register_unit(const char *unit_name, const char *unit_type);
int var_lib_systemd_get_state(var_lib_systemd_state_t *state);

#endif
