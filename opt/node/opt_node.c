#include "opt_node.h"

int opt_node_init(void)
{
    return 0;
}

int opt_node_register_app(opt_node_app_t *app)
{
    if (!app)
    {
        return -1;
    }
    return 0;
}

int opt_node_unregister_app(uint32_t app_id)
{
    return 0;
}

int opt_node_start_app(uint32_t app_id)
{
    return 0;
}

int opt_node_stop_app(uint32_t app_id)
{
    return 0;
}

int opt_node_get_app_info(uint32_t app_id, opt_node_app_t *app)
{
    if (!app)
    {
        return -1;
    }
    return 0;
}

int opt_node_get_state(opt_node_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
