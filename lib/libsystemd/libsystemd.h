#ifndef LIBSYSTEMD_H
#define LIBSYSTEMD_H

#include <stdint.h>

typedef void sd_bus;
typedef void sd_event;

int libsystemd_init(void);
int libsystemd_bus_open_system(sd_bus **bus);
int libsystemd_bus_close(sd_bus *bus);
int libsystemd_event_new(sd_event **event);
int libsystemd_event_unref(sd_event *event);
int libsystemd_bus_call_method(sd_bus *bus, const char *destination, const char *path, const char *interface, const char *member);

#endif
