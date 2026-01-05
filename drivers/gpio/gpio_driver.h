#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <stdint.h>

#define GPIO_IN 0
#define GPIO_OUT 1

typedef struct
{
    uint32_t pin;
    uint32_t direction;
    uint32_t value;
} gpio_pin_t;

int gpio_init(void);
int gpio_request_pin(uint32_t pin);
int gpio_release_pin(uint32_t pin);
int gpio_set_direction(gpio_pin_t *pin, uint32_t direction);
int gpio_set_value(gpio_pin_t *pin, uint32_t value);
int gpio_get_value(gpio_pin_t *pin, uint32_t *value);

#endif
