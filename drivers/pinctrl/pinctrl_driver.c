#include "pinctrl_driver.h"

int pinctrl_init(void)
{
    return 0;
}

int pinctrl_register_pin(pinctrl_pin_t *pin)
{
    if (!pin)
    {
        return -1;
    }
    return 0;
}

int pinctrl_set_mux(uint32_t pin, uint32_t function)
{
    return 0;
}

int pinctrl_set_config(uint32_t pin, uint32_t config)
{
    return 0;
}

int pinctrl_get_function(uint32_t pin, uint32_t *function)
{
    if (!function)
    {
        return -1;
    }
    *function = 0;
    return 0;
}
