#include "gpio_driver.h"

int gpio_init(void)
{
    return 0;
}

int gpio_request_pin(uint32_t pin)
{
    return 0;
}

int gpio_release_pin(uint32_t pin)
{
    return 0;
}

int gpio_set_direction(gpio_pin_t *pin, uint32_t direction)
{
    if (!pin)
    {
        return -1;
    }
    pin->direction = direction;
    return 0;
}

int gpio_set_value(gpio_pin_t *pin, uint32_t value)
{
    if (!pin)
    {
        return -1;
    }
    pin->value = value;
    return 0;
}

int gpio_get_value(gpio_pin_t *pin, uint32_t *value)
{
    if (!pin || !value)
    {
        return -1;
    }
    *value = pin->value;
    return 0;
}
