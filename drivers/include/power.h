#ifndef POWER_H
#define POWER_H

#include <stdint.h>

typedef struct
{
    uint32_t power_domain_id;
    const char *name;
    uint32_t flags;
} power_domain_t;

#define POWER_STATE_ON 1
#define POWER_STATE_SLEEP 2
#define POWER_STATE_OFF 0

#endif
