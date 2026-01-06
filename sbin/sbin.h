#ifndef SBIN_H
#define SBIN_H

#include <stdint.h>

int sbin_init(void);
int sbin_start_service(const char *service);
int sbin_stop_service(const char *service);
int sbin_restart_service(const char *service);
int sbin_get_service_status(const char *service, uint32_t *status);
int sbin_enable_service(const char *service);

#endif
