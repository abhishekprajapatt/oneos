#ifndef BLUETOOTH_DRIVER_H
#define BLUETOOTH_DRIVER_H

#include <stdint.h>

#define BT_ADDR_LEN 6
#define BT_MAX_DEVICES 32

typedef struct
{
    uint8_t addr[BT_ADDR_LEN];
    uint8_t name[248];
    int8_t rssi;
    uint8_t connected;
} bt_device_t;

typedef struct
{
    uint32_t device_id;
    uint32_t power_state;
    uint32_t discoverable;
} bt_adapter_t;

int bt_init(void);
int bt_power_on(bt_adapter_t *adapter);
int bt_power_off(bt_adapter_t *adapter);
int bt_scan_devices(bt_adapter_t *adapter, bt_device_t *devices);
int bt_connect(bt_adapter_t *adapter, uint8_t *addr);

#endif
