#ifndef CHAR_WATCHDOG_DRIVER_H
#define CHAR_WATCHDOG_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t device_id;
    uint32_t timeout;
    uint32_t pretimeout;
    uint32_t enabled;
    uint32_t bootstatus;
} char_watchdog_device_t;

int char_watchdog_init(void);
int char_watchdog_register(char_watchdog_device_t *dev);
int char_watchdog_unregister(uint32_t device_id);
int char_watchdog_start(char_watchdog_device_t *dev);
int char_watchdog_stop(char_watchdog_device_t *dev);
int char_watchdog_ping(char_watchdog_device_t *dev);
int char_watchdog_set_timeout(char_watchdog_device_t *dev, uint32_t timeout);

#endif
