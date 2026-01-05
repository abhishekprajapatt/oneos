#ifndef ACPI_PMIC_H
#define ACPI_PMIC_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    uint32_t voltage;
    uint32_t current;
    uint32_t temperature;
} pmic_info_t;

int pmic_init(void);
int pmic_read_voltage(uint32_t id, uint32_t *voltage);
int pmic_read_current(uint32_t id, uint32_t *current);
int pmic_read_temperature(uint32_t id, uint32_t *temp);
int pmic_set_voltage(uint32_t id, uint32_t voltage);

#endif
