#include "services_dbus.h"

int services_dbus_init(void)
{
    return 0;
}

int services_dbus_connect(const char *address)
{
    if (!address)
    {
        return -1;
    }
    return 0;
}

int services_dbus_disconnect(void)
{
    return 0;
}

int services_dbus_send_message(services_dbus_message_t *message)
{
    if (!message)
    {
        return -1;
    }
    return 0;
}

int services_dbus_register_signal(const char *interface, const char *signal)
{
    if (!interface || !signal)
    {
        return -1;
    }
    return 0;
}

int services_dbus_unregister_signal(const char *interface, const char *signal)
{
    if (!interface || !signal)
    {
        return -1;
    }
    return 0;
}

int services_dbus_get_state(services_dbus_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
