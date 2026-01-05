#include "bluetooth_driver.h"

int bt_init(void)
{
    return 0;
}

int bt_power_on(bt_adapter_t *adapter)
{
    if (!adapter)
    {
        return -1;
    }
    adapter->power_state = 1;
    return 0;
}

int bt_power_off(bt_adapter_t *adapter)
{
    if (!adapter)
    {
        return -1;
    }
    adapter->power_state = 0;
    return 0;
}

int bt_scan_devices(bt_adapter_t *adapter, bt_device_t *devices)
{
    if (!adapter || !devices)
    {
        return -1;
    }
    return 0;
}

int bt_connect(bt_adapter_t *adapter, uint8_t *addr)
{
    if (!adapter || !addr)
    {
        return -1;
    }
    return 0;
}
