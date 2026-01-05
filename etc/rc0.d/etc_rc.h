#ifndef ETC_RC_H
#define ETC_RC_H

#include <stdint.h>

typedef struct
{
    uint32_t service_id;
    const char *service_name;
    uint32_t runlevel;
    uint32_t priority;
} etc_rc_service_t;

typedef struct
{
    uint32_t runlevel;
    uint32_t service_count;
    uint32_t enabled_count;
} etc_rc_state_t;

int etc_rc_init(void);
int etc_rc_register_service(etc_rc_service_t *service);
int etc_rc_unregister_service(uint32_t service_id);
int etc_rc_enable_service(uint32_t service_id);
int etc_rc_disable_service(uint32_t service_id);
int etc_rc_start_service(uint32_t service_id);
int etc_rc_stop_service(uint32_t service_id);

#endif
