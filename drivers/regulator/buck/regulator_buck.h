#ifndef REGULATOR_BUCK_H
#define REGULATOR_BUCK_H

#include <stdint.h>

typedef struct
{
    uint32_t regulator_id;
    uint32_t base_address;
    uint32_t min_voltage;
    uint32_t max_voltage;
    uint32_t flags;
} regulator_buck_device_t;

typedef struct
{
    uint32_t voltage;
    uint32_t current_limit;
    uint8_t mode;
} regulator_buck_config_t;

int regulator_buck_init(void);
int regulator_buck_probe(regulator_buck_device_t *dev);
int regulator_buck_set_voltage(regulator_buck_device_t *dev, uint32_t voltage);
int regulator_buck_get_voltage(regulator_buck_device_t *dev, uint32_t *voltage);
int regulator_buck_enable(regulator_buck_device_t *dev);
int regulator_buck_disable(regulator_buck_device_t *dev);

#endif
