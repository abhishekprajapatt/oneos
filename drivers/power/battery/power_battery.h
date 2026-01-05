#ifndef POWER_BATTERY_H
#define POWER_BATTERY_H

#include <stdint.h>

typedef struct
{
    uint32_t battery_id;
    uint32_t base_address;
    uint32_t capacity;
    uint32_t flags;
} power_battery_device_t;

typedef struct
{
    int32_t voltage;
    int32_t current;
    uint8_t soc;
    uint8_t status;
    int32_t temperature;
} power_battery_info_t;

int power_battery_init(void);
int power_battery_probe(power_battery_device_t *dev);
int power_battery_get_info(power_battery_device_t *dev, power_battery_info_t *info);
int power_battery_set_charge_current(power_battery_device_t *dev, uint32_t current);
int power_battery_enable_charging(power_battery_device_t *dev);
int power_battery_disable_charging(power_battery_device_t *dev);

#endif
