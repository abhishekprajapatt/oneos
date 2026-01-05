#ifndef INIT_SERVICE_H
#define INIT_SERVICE_H

#include <stdint.h>

typedef struct
{
    uint32_t service_id;
    const char *service_name;
    uint32_t service_type;
    uint32_t restart_policy;
} init_service_descriptor_t;

typedef struct
{
    uint32_t service_count;
    uint32_t running_count;
    uint32_t failed_count;
} init_service_state_t;

int init_service_register(init_service_descriptor_t *descriptor);
int init_service_unregister(uint32_t service_id);
int init_service_start(uint32_t service_id);
int init_service_stop(uint32_t service_id);
int init_service_restart(uint32_t service_id);
int init_service_get_status(uint32_t service_id);

#endif
