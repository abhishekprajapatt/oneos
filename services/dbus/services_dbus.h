#ifndef SERVICES_DBUS_H
#define SERVICES_DBUS_H

#include <stdint.h>

typedef struct
{
    uint32_t message_id;
    const char *sender;
    const char *destination;
    const char *interface;
    const char *member;
} services_dbus_message_t;

typedef struct
{
    uint32_t total_messages;
    uint32_t sent_messages;
    uint32_t received_messages;
    uint32_t failed_messages;
} services_dbus_state_t;

int services_dbus_init(void);
int services_dbus_connect(const char *address);
int services_dbus_disconnect(void);
int services_dbus_send_message(services_dbus_message_t *message);
int services_dbus_register_signal(const char *interface, const char *signal);
int services_dbus_unregister_signal(const char *interface, const char *signal);
int services_dbus_get_state(services_dbus_state_t *state);

#endif
