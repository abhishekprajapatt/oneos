#include "net_wireless.h"

int net_wireless_init(void)
{
    return 0;
}

int net_wireless_register_device(net_wireless_device_t *device)
{
    if (!device)
    {
        return -1;
    }
    return 0;
}

int net_wireless_unregister_device(uint32_t device_id)
{
    return 0;
}

int net_wireless_scan_networks(uint32_t device_id)
{
    return 0;
}

int net_wireless_connect(uint32_t device_id, const char *ssid, const char *password)
{
    if (!ssid || !password)
    {
        return -1;
    }
    return 0;
}

int net_wireless_disconnect(uint32_t device_id)
{
    return 0;
}
