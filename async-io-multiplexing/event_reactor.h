#ifndef EVENT_REACTOR_H
#define EVENT_REACTOR_H

#include <stdint.h>
#include <stddef.h>

#define MAX_EVENT_SOURCES 8192
#define MAX_EVENT_HANDLERS 512
#define MAX_EVENTS_PER_POLL 4096

typedef enum
{
    EVENT_TYPE_READ,
    EVENT_TYPE_WRITE,
    EVENT_TYPE_ERROR,
    EVENT_TYPE_TIMEOUT,
    EVENT_TYPE_SIGNAL,
    EVENT_TYPE_CUSTOM
} EventType;

typedef enum
{
    EVENT_STATE_INACTIVE,
    EVENT_STATE_REGISTERED,
    EVENT_STATE_FIRED,
    EVENT_STATE_PROCESSING,
    EVENT_STATE_READY
} EventState;

typedef int (*EventHandler)(int fd, EventType type, void *user_data);

typedef struct
{
    uint32_t event_source_id;
    int fd;
    EventType event_type;
    EventState state;

    EventHandler handler;
    void *user_data;

    uint32_t event_count;
    uint64_t last_event_time;
    uint8_t is_oneshot;
    uint8_t is_edge_triggered;
} EventSource;

typedef struct
{
    uint32_t event_id;
    uint32_t source_id;
    EventType type;
    uint64_t timestamp;
    int fd;
    uint32_t event_flags;
} Event;

typedef struct
{
    uint32_t reactor_id;
    uint32_t event_source_count;
    EventSource event_sources[MAX_EVENT_SOURCES];

    Event event_queue[MAX_EVENTS_PER_POLL];
    uint32_t event_queue_front;
    uint32_t event_queue_rear;
    uint32_t pending_events;

    uint32_t total_events_processed;
    uint64_t reactor_start_time;
    float events_per_second;

    uint8_t is_running;
} EventReactor;

EventReactor *event_reactor_create(void);
int event_reactor_destroy(EventReactor *reactor);

int event_register(EventReactor *reactor, int fd, EventType type, EventHandler handler, void *user_data);
int event_unregister(EventReactor *reactor, int fd);

int event_modify(EventReactor *reactor, int fd, EventType type, uint32_t flags);
int event_set_oneshot(EventReactor *reactor, int fd, uint8_t oneshot);
int event_set_edge_triggered(EventReactor *reactor, int fd, uint8_t edge_triggered);

int event_dispatch(EventReactor *reactor, int timeout_ms);
int event_process_queue(EventReactor *reactor);

int event_trigger_custom(EventReactor *reactor, uint32_t source_id, uint32_t flags);
int event_handle_timeout(EventReactor *reactor, uint32_t source_id);

typedef struct
{
    uint32_t reactor_id;
    uint32_t registered_sources;
    uint32_t pending_events;
    uint32_t total_processed;
    float events_per_second;
} ReactorMetrics;

int event_reactor_get_metrics(EventReactor *reactor, ReactorMetrics *metrics);

#endif
