#include "services_network.h"

int services_network_init(void)
{
    return 0;
}

int services_network_configure_interface(services_network_interface_t *iface)
{
    if (!iface)
    {
        return -1;
    }
    return 0;
}

int services_network_enable_interface(uint32_t interface_id)
{
    return 0;
}

int services_network_disable_interface(uint32_t interface_id)
{
    return 0;
}

int services_network_get_interface_stats(uint32_t interface_id, services_network_interface_t *iface)
{
    if (!iface)
    {
        return -1;
    }
    return 0;
}

int services_network_set_gateway(uint32_t gateway_addr)
{
    return 0;
}

int services_network_get_state(services_network_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
