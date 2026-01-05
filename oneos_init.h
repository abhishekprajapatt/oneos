#ifndef ONEOS_INIT_H
#define ONEOS_INIT_H

#include <stdint.h>

typedef struct
{
    uint32_t init_id;
    const char *init_name;
    const char *version;
} oneos_init_config_t;

int oneos_init_system(void);
int oneos_init_services(void);
int oneos_init_daemon_processes(void);
int oneos_init_filesystem(void);
int oneos_init_network(void);
int oneos_init_user_environment(void);

#endif
