#include "net.h"

int net_init(void)
{
    return 0;
}

int net_register_interface(const char *name, const char *driver)
{
    if (!name || !driver)
    {
        return -1;
    }
    return 0;
}

int net_set_ipv4_addr(const char *iface, net_ipv4_addr_t *addr)
{
    if (!iface || !addr)
    {
        return -1;
    }
    return 0;
}

int net_get_ipv4_addr(const char *iface, net_ipv4_addr_t *addr)
{
    if (!iface || !addr)
    {
        return -1;
    }
    return 0;
}

int net_send_packet(const char *iface, void *packet, uint32_t size)
{
    if (!iface || !packet || size == 0)
    {
        return -1;
    }
    return 0;
}

int net_recv_packet(const char *iface, void **packet, uint32_t *size)
{
    if (!iface || !packet || !size)
    {
        return -1;
    }
    return 0;
}
