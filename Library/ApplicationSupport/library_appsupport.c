#include "library_appsupport.h"

int library_appsupport_init(void)
{
    return 0;
}

int library_appsupport_register_app(library_appsupport_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int library_appsupport_unregister_app(uint32_t app_id)
{
    return 0;
}

int library_appsupport_get_app_data(uint32_t app_id, library_appsupport_entry_t *entry)
{
    if (!entry)
    {
        return -1;
    }
    return 0;
}

int library_appsupport_cleanup(uint32_t app_id)
{
    return 0;
}

int library_appsupport_get_state(library_appsupport_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
