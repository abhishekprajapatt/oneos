#ifndef NET_CORE_H
#define NET_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t interface_id;
    const char *interface_name;
    uint32_t mtu;
    uint32_t status;
} net_core_interface_t;

typedef struct
{
    uint32_t interface_count;
    uint32_t active_interfaces;
    uint64_t bytes_sent;
    uint64_t bytes_received;
} net_core_state_t;

int net_core_init(void);
int net_core_register_interface(net_core_interface_t *interface);
int net_core_unregister_interface(uint32_t interface_id);
int net_core_transmit_frame(uint32_t interface_id, uint8_t *frame, uint32_t size);
int net_core_receive_frame(uint32_t interface_id, uint8_t *frame, uint32_t *size);
int net_core_get_interface_info(uint32_t interface_id, net_core_interface_t *info);

#endif
