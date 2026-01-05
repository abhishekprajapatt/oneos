#include "net_wl_broadcom.h"

int net_wl_broadcom_init(void)
{
    return 0;
}

int net_wl_broadcom_probe(void)
{
    return 0;
}

int net_wl_broadcom_open(net_wl_broadcom_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_broadcom_close(net_wl_broadcom_device_t *dev)
{
    if (!dev)
    {
        return -1;
    }
    return 0;
}

int net_wl_broadcom_scan(net_wl_broadcom_device_t *dev, net_wl_broadcom_scan_t *scan)
{
    if (!dev || !scan)
    {
        return -1;
    }
    return 0;
}

int net_wl_broadcom_connect(net_wl_broadcom_device_t *dev, uint8_t *ssid, uint32_t ssid_len)
{
    if (!dev || !ssid || ssid_len == 0)
    {
        return -1;
    }
    return 0;
}
