#ifndef NET_H
#define NET_H

#include <stdint.h>

typedef struct
{
    uint8_t addr[4];
} net_ipv4_addr_t;

int net_init(void);
int net_register_interface(const char *name, const char *driver);
int net_set_ipv4_addr(const char *iface, net_ipv4_addr_t *addr);
int net_get_ipv4_addr(const char *iface, net_ipv4_addr_t *addr);
int net_send_packet(const char *iface, void *packet, uint32_t size);
int net_recv_packet(const char *iface, void **packet, uint32_t *size);

#endif
