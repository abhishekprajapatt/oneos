#ifndef CLOCKSOURCE_DRIVER_H
#define CLOCKSOURCE_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint64_t frequency;
    uint64_t counter;
} clocksource_t;

int clocksource_init(void);
int clocksource_register(clocksource_t *cs);
int clocksource_get_time(uint32_t id, uint64_t *time);
int clocksource_set_frequency(uint32_t id, uint64_t freq);

#endif
