#ifndef WATCHDOG_DRIVER_H
#define WATCHDOG_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t timeout;
    uint32_t pretimeout;
    uint32_t enabled;
} watchdog_device_t;

int watchdog_init(void);
int watchdog_register(watchdog_device_t *dev);
int watchdog_start(watchdog_device_t *dev);
int watchdog_stop(watchdog_device_t *dev);
int watchdog_ping(watchdog_device_t *dev);
int watchdog_set_timeout(watchdog_device_t *dev, uint32_t timeout);

#endif
