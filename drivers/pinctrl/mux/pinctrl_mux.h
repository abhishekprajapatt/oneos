#ifndef PINCTRL_MUX_H
#define PINCTRL_MUX_H

#include <stdint.h>

typedef struct
{
    uint32_t pinctrl_id;
    uint32_t base_address;
    uint32_t pin_count;
    uint32_t flags;
} pinctrl_mux_device_t;

typedef struct
{
    uint32_t pin;
    uint8_t mux_mode;
    uint8_t pull_config;
    uint32_t pad_control;
} pinctrl_mux_config_t;

int pinctrl_mux_init(void);
int pinctrl_mux_probe(pinctrl_mux_device_t *dev);
int pinctrl_mux_config_pin(pinctrl_mux_device_t *dev, pinctrl_mux_config_t *cfg);
int pinctrl_mux_get_config(pinctrl_mux_device_t *dev, uint32_t pin, pinctrl_mux_config_t *cfg);
int pinctrl_mux_set_drive_strength(pinctrl_mux_device_t *dev, uint32_t pin, uint8_t strength);
int pinctrl_mux_enable_slew_rate(pinctrl_mux_device_t *dev, uint32_t pin);

#endif
