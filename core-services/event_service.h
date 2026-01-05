#ifndef EVENT_SERVICE_H
#define EVENT_SERVICE_H

#include <stdint.h>

typedef struct
{
    uint32_t event_id;
    const char *event_name;
    uint32_t event_type;
    uint64_t timestamp;
    void *event_data;
} event_info_t;

int event_service_init(void);
int event_service_register_listener(const char *event_name, uint64_t pid);
int event_service_unregister_listener(const char *event_name, uint64_t pid);
int event_service_emit_event(event_info_t *event);
int event_service_get_pending_events(uint64_t pid, event_info_t *events, uint32_t *count);
int event_service_clear_events(uint64_t pid);
int event_service_wait_event(const char *event_name, uint32_t timeout);

#endif
