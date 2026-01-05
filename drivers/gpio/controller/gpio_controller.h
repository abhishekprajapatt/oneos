#ifndef GPIO_CONTROLLER_H
#define GPIO_CONTROLLER_H

#include <stdint.h>

typedef struct
{
    uint32_t gpio_id;
    uint32_t base_address;
    uint32_t pin_count;
    uint32_t flags;
} gpio_controller_device_t;

typedef struct
{
    uint32_t pin;
    uint8_t direction;
    uint8_t value;
    uint8_t pull_config;
} gpio_controller_config_t;

int gpio_controller_init(void);
int gpio_controller_probe(gpio_controller_device_t *dev);
int gpio_controller_config_pin(gpio_controller_device_t *dev, gpio_controller_config_t *cfg);
int gpio_controller_set_value(gpio_controller_device_t *dev, uint32_t pin, uint8_t value);
int gpio_controller_get_value(gpio_controller_device_t *dev, uint32_t pin, uint8_t *value);
int gpio_controller_configure_interrupt(gpio_controller_device_t *dev, uint32_t pin, uint8_t trigger);

#endif
