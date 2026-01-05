#ifndef NETWORK_BLUETOOTH_H
#define NETWORK_BLUETOOTH_H

#include <stdint.h>

typedef struct
{
    uint32_t adapter_id;
    uint8_t bd_addr[6];
    uint32_t flags;
    uint32_t dev_class;
} network_bt_device_t;

typedef struct
{
    uint8_t bd_addr[6];
    int8_t rssi;
    uint8_t *eir_data;
    uint32_t eir_len;
} network_bt_inquiry_result_t;

int network_bt_init(void);
int network_bt_enable_adapter(network_bt_device_t *dev);
int network_bt_disable_adapter(network_bt_device_t *dev);
int network_bt_start_discovery(network_bt_device_t *dev);
int network_bt_stop_discovery(network_bt_device_t *dev);
int network_bt_create_connection(network_bt_device_t *dev, uint8_t *bd_addr);
int network_bt_get_device_info(network_bt_device_t *dev, void *info);

#endif
