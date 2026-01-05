#ifndef POWER_DRIVER_H
#define POWER_DRIVER_H

#include <stdint.h>

#define POWER_STATE_ON 0
#define POWER_STATE_SLEEP 1
#define POWER_STATE_SUSPEND 2
#define POWER_STATE_HIBERNATE 3

typedef struct
{
    uint32_t device_id;
    uint32_t power_state;
    uint32_t wake_sources;
} power_device_t;

int power_init(void);
int power_register_device(power_device_t *dev);
int power_set_state(power_device_t *dev, uint32_t state);
int power_get_state(power_device_t *dev, uint32_t *state);
int power_wake_lock(power_device_t *dev);
int power_wake_unlock(power_device_t *dev);

#endif
