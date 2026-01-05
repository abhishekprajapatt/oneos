#ifndef SBIN_SYSTEMCTL_H
#define SBIN_SYSTEMCTL_H

#include <stdint.h>

typedef struct
{
    uint32_t service_id;
    const char *service_name;
    uint32_t service_status;
    uint32_t service_enabled;
} sbin_systemctl_service_t;

typedef struct
{
    uint32_t total_services;
    uint32_t running_services;
    uint32_t stopped_services;
    uint32_t enabled_services;
} sbin_systemctl_state_t;

int sbin_systemctl_init(void);
int sbin_systemctl_start_service(const char *service_name);
int sbin_systemctl_stop_service(const char *service_name);
int sbin_systemctl_restart_service(const char *service_name);
int sbin_systemctl_enable_service(const char *service_name);
int sbin_systemctl_disable_service(const char *service_name);
int sbin_systemctl_get_service_status(const char *service_name, sbin_systemctl_service_t *service);
int sbin_systemctl_get_state(sbin_systemctl_state_t *state);

#endif
