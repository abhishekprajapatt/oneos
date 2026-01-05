#ifndef SERVICES_CORE_H
#define SERVICES_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t service_id;
    const char *service_name;
    uint32_t service_state;
    uint32_t service_priority;
    uint64_t service_pid;
} services_core_service_t;

typedef struct
{
    uint32_t total_services;
    uint32_t running_services;
    uint32_t stopped_services;
    uint32_t failed_services;
} services_core_state_t;

int services_core_init(void);
int services_core_register_service(services_core_service_t *service);
int services_core_unregister_service(uint32_t service_id);
int services_core_start_service(uint32_t service_id);
int services_core_stop_service(uint32_t service_id);
int services_core_restart_service(uint32_t service_id);
int services_core_get_service_info(uint32_t service_id, services_core_service_t *service);
int services_core_get_state(services_core_state_t *state);

#endif
