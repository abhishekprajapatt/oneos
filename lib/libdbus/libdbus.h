#ifndef LIBDBUS_H
#define LIBDBUS_H

#include <stdint.h>

typedef struct
{
    const char *bus_name;
    const char *object_path;
    const char *interface;
} libdbus_connection_t;

int libdbus_init(void);
int libdbus_connect(libdbus_connection_t *conn);
int libdbus_disconnect(libdbus_connection_t *conn);
int libdbus_send_message(libdbus_connection_t *conn, const void *message, uint32_t size);
int libdbus_receive_message(libdbus_connection_t *conn, void *message, uint32_t *size);
int libdbus_call_method(libdbus_connection_t *conn, const char *method, void *args);

#endif
