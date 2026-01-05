#include "net_wl_intel.h"

int net_wl_intel_init(void)
{
    return 0;
}

int net_wl_intel_probe(void)
{
    return 0;
}

int net_wl_intel_open(net_wl_intel_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_intel_close(net_wl_intel_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_intel_transmit(net_wl_intel_device_t *dev, void *packet, net_wl_intel_tx_params_t *params)
{
    if (!dev || !packet || !params)
    {
        return -1;
    }
    return 0;
}

int net_wl_intel_set_power(net_wl_intel_device_t *dev, uint32_t power_mode)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
