#ifndef TIMER_DRIVER_H
#define TIMER_DRIVER_H

#include <stdint.h>

int timer_driver_init(void);
int timer_driver_set_timeout(uint32_t milliseconds);
int timer_driver_cancel_timeout(void);
int timer_driver_get_ticks(uint64_t *ticks);
int timer_driver_sleep(uint32_t milliseconds);

#endif
