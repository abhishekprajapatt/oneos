#ifndef SERVICES_H
#define SERVICES_H

#include <stdint.h>

typedef struct
{
    uint32_t service_id;
    uint32_t status;
} service_t;

int services_init(void);
int services_register(const char *name, void (*handler)(void));
int services_start(const char *name);
int services_stop(const char *name);
int services_get_status(const char *name, service_t *service);
int services_restart(const char *name);

#endif
