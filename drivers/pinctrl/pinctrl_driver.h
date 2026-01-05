#ifndef PINCTRL_DRIVER_H
#define PINCTRL_DRIVER_H

#include <stdint.h>

typedef struct
{
    uint32_t pin;
    uint32_t function;
    uint32_t config;
} pinctrl_pin_t;

int pinctrl_init(void);
int pinctrl_register_pin(pinctrl_pin_t *pin);
int pinctrl_set_mux(uint32_t pin, uint32_t function);
int pinctrl_set_config(uint32_t pin, uint32_t config);
int pinctrl_get_function(uint32_t pin, uint32_t *function);

#endif
