#include "gpio_controller.h"

int gpio_controller_init(void)
{
    return 0;
}

int gpio_controller_probe(gpio_controller_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int gpio_controller_config_pin(gpio_controller_device_t *dev, gpio_controller_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int gpio_controller_set_value(gpio_controller_device_t *dev, uint32_t pin, uint8_t value)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int gpio_controller_get_value(gpio_controller_device_t *dev, uint32_t pin, uint8_t *value)
{
    if (!dev || !value)
    {
        return -1;
    }
    return 0;
}

int gpio_controller_configure_interrupt(gpio_controller_device_t *dev, uint32_t pin, uint8_t trigger)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
