#include "init_network.h"

int init_network_load_config(const char *config_file)
{
    if (!config_file)
    {
        return -1;
    }
    return 0;
}

int init_network_add_interface(init_network_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}

int init_network_remove_interface(uint32_t interface_id)
{
    return 0;
}

int init_network_configure_interface(uint32_t interface_id)
{
    return 0;
}

int init_network_bring_up_interface(uint32_t interface_id)
{
    return 0;
}

int init_network_bring_down_interface(uint32_t interface_id)
{
    return 0;
}
