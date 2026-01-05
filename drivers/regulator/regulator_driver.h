#ifndef REGULATOR_DRIVER_H
#define REGULATOR_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t voltage;
    uint32_t min_voltage;
    uint32_t max_voltage;
    uint32_t enabled;
} regulator_t;

int regulator_init(void);
int regulator_register(regulator_t *reg);
int regulator_enable(uint32_t id);
int regulator_disable(uint32_t id);
int regulator_set_voltage(uint32_t id, uint32_t voltage);
int regulator_get_voltage(uint32_t id, uint32_t *voltage);

#endif
