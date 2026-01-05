#ifndef SYSTEM_CORE_H
#define SYSTEM_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t component_id;
    const char *component_name;
    const char *version;
    uint32_t status;
    uint64_t uptime;
} system_core_component_t;

typedef struct
{
    uint32_t total_components;
    uint32_t active_components;
    uint32_t inactive_components;
    uint64_t system_uptime;
} system_core_state_t;

int system_core_init(void);
int system_core_register_component(system_core_component_t *component);
int system_core_unregister_component(uint32_t component_id);
int system_core_get_component(uint32_t component_id, system_core_component_t *component);
int system_core_set_component_status(uint32_t component_id, uint32_t status);
int system_core_get_system_uptime(uint64_t *uptime);
int system_core_get_state(system_core_state_t *state);

#endif
