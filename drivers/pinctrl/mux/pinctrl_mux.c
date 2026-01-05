#include "pinctrl_mux.h"

int pinctrl_mux_init(void)
{
    return 0;
}

int pinctrl_mux_probe(pinctrl_mux_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int pinctrl_mux_config_pin(pinctrl_mux_device_t *dev, pinctrl_mux_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int pinctrl_mux_get_config(pinctrl_mux_device_t *dev, uint32_t pin, pinctrl_mux_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int pinctrl_mux_set_drive_strength(pinctrl_mux_device_t *dev, uint32_t pin, uint8_t strength)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int pinctrl_mux_enable_slew_rate(pinctrl_mux_device_t *dev, uint32_t pin)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
