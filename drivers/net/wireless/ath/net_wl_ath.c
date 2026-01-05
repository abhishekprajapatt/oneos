#include "net_wl_ath.h"

int net_wl_ath_init(void)
{
    return 0;
}

int net_wl_ath_probe(void)
{
    return 0;
}

int net_wl_ath_open(net_wl_ath_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_ath_close(net_wl_ath_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_ath_configure(net_wl_ath_device_t *dev, net_wl_ath_config_t *cfg)
{
    if (!dev || !cfg)
    {
        return -1;
    }
    return 0;
}

int net_wl_ath_scan(net_wl_ath_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}
