#ifndef SERVICE_MANAGER_H
#define SERVICE_MANAGER_H

#include <stdint.h>

typedef struct
{
    uint32_t service_id;
    const char *service_name;
    uint32_t service_type;
    uint32_t status;
    uint32_t start_order;
} service_info_t;

int service_manager_init(void);
int service_manager_start_service(const char *service_name);
int service_manager_stop_service(uint32_t service_id);
int service_manager_enable_service(uint32_t service_id);
int service_manager_disable_service(uint32_t service_id);
int service_manager_get_status(uint32_t service_id);
int service_manager_list_services(service_info_t *services, uint32_t *count);

#endif
