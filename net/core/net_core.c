#include "net_core.h"

int net_core_init(void)
{
    return 0;
}

int net_core_register_interface(net_core_interface_t *interface)
{
    if (!interface)
    {
        return -1;
    }
    return 0;
}

int net_core_unregister_interface(uint32_t interface_id)
{
    return 0;
}

int net_core_transmit_frame(uint32_t interface_id, uint8_t *frame, uint32_t size)
{
    if (!frame)
    {
        return -1;
    }
    return 0;
}

int net_core_receive_frame(uint32_t interface_id, uint8_t *frame, uint32_t *size)
{
    if (!frame || !size)
    {
        return -1;
    }
    return 0;
}

int net_core_get_interface_info(uint32_t interface_id, net_core_interface_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}
