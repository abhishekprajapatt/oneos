#include "event_reactor.h"
#include <stdlib.h>
#include <string.h>

static EventReactor *global_event_reactor = NULL;
static uint32_t next_event_source_id = 1;
static uint32_t next_event_id = 1;

EventReactor *event_reactor_create(void)
{
    EventReactor *reactor = (EventReactor *)malloc(sizeof(EventReactor));
    if (!reactor)
        return NULL;

    memset(reactor, 0, sizeof(EventReactor));
    reactor->reactor_id = 1;
    reactor->event_source_count = 0;
    reactor->event_queue_front = 0;
    reactor->event_queue_rear = 0;
    reactor->pending_events = 0;
    reactor->total_events_processed = 0;
    reactor->reactor_start_time = 0;
    reactor->events_per_second = 0.0f;
    reactor->is_running = 0;

    global_event_reactor = reactor;
    return reactor;
}

int event_reactor_destroy(EventReactor *reactor)
{
    if (!reactor)
        return -1;

    free(reactor);
    global_event_reactor = NULL;
    return 0;
}

int event_register(EventReactor *reactor, int fd, EventType type, EventHandler handler, void *user_data)
{
    if (!reactor || !handler || fd < 0)
        return -1;

    if (reactor->event_source_count >= MAX_EVENT_SOURCES)
        return -1;

    EventSource *source = NULL;
    for (uint32_t i = 0; i < reactor->event_source_count; i++)
    {
        if (reactor->event_sources[i].fd == fd)
        {
            source = &reactor->event_sources[i];
            break;
        }
    }

    if (!source)
    {
        source = &reactor->event_sources[reactor->event_source_count];
        reactor->event_source_count++;
    }

    source->event_source_id = next_event_source_id++;
    source->fd = fd;
    source->event_type = type;
    source->state = EVENT_STATE_REGISTERED;
    source->handler = handler;
    source->user_data = user_data;
    source->event_count = 0;
    source->last_event_time = 0;
    source->is_oneshot = 0;
    source->is_edge_triggered = 0;

    return source->event_source_id;
}

int event_unregister(EventReactor *reactor, int fd)
{
    if (!reactor || fd < 0)
        return -1;

    for (uint32_t i = 0; i < reactor->event_source_count; i++)
    {
        if (reactor->event_sources[i].fd == fd)
        {
            memmove(&reactor->event_sources[i], &reactor->event_sources[i + 1],
                    sizeof(EventSource) * (reactor->event_source_count - i - 1));
            reactor->event_source_count--;
            return 0;
        }
    }

    return -1;
}

int event_modify(EventReactor *reactor, int fd, EventType type, uint32_t flags)
{
    if (!reactor || fd < 0)
        return -1;

    for (uint32_t i = 0; i < reactor->event_source_count; i++)
    {
        if (reactor->event_sources[i].fd == fd)
        {
            reactor->event_sources[i].event_type = type;
            if (flags & 0x01)
                reactor->event_sources[i].is_oneshot = 1;
            if (flags & 0x02)
                reactor->event_sources[i].is_edge_triggered = 1;
            return 0;
        }
    }

    return -1;
}

int event_set_oneshot(EventReactor *reactor, int fd, uint8_t oneshot)
{
    if (!reactor || fd < 0)
        return -1;

    for (uint32_t i = 0; i < reactor->event_source_count; i++)
    {
        if (reactor->event_sources[i].fd == fd)
        {
            reactor->event_sources[i].is_oneshot = oneshot;
            return 0;
        }
    }

    return -1;
}

int event_set_edge_triggered(EventReactor *reactor, int fd, uint8_t edge_triggered)
{
    if (!reactor || fd < 0)
        return -1;

    for (uint32_t i = 0; i < reactor->event_source_count; i++)
    {
        if (reactor->event_sources[i].fd == fd)
        {
            reactor->event_sources[i].is_edge_triggered = edge_triggered;
            return 0;
        }
    }

    return -1;
}

int event_dispatch(EventReactor *reactor, int timeout_ms)
{
    if (!reactor)
        return -1;

    reactor->is_running = 1;
    uint32_t events_found = 0;

    for (uint32_t i = 0; i < reactor->event_source_count && events_found < MAX_EVENTS_PER_POLL; i++)
    {
        EventSource *source = &reactor->event_sources[i];
        if (source->state == EVENT_STATE_REGISTERED)
        {
            source->state = EVENT_STATE_READY;

            Event *evt = &reactor->event_queue[reactor->event_queue_rear];
            evt->event_id = next_event_id++;
            evt->source_id = source->event_source_id;
            evt->type = source->event_type;
            evt->timestamp = 0;
            evt->fd = source->fd;
            evt->event_flags = 0;

            reactor->event_queue_rear = (reactor->event_queue_rear + 1) % MAX_EVENTS_PER_POLL;
            reactor->pending_events++;
            events_found++;
        }
    }

    return events_found;
}

int event_process_queue(EventReactor *reactor)
{
    if (!reactor)
        return -1;

    uint32_t processed = 0;

    while (reactor->pending_events > 0 && processed < MAX_EVENTS_PER_POLL)
    {
        Event *evt = &reactor->event_queue[reactor->event_queue_front];

        for (uint32_t i = 0; i < reactor->event_source_count; i++)
        {
            if (reactor->event_sources[i].event_source_id == evt->source_id)
            {
                EventSource *source = &reactor->event_sources[i];

                if (source->handler)
                {
                    source->handler(source->fd, evt->type, source->user_data);
                }

                source->event_count++;
                source->last_event_time = 0;
                source->state = EVENT_STATE_PROCESSING;

                if (source->is_oneshot)
                {
                    source->state = EVENT_STATE_INACTIVE;
                }
                else
                {
                    source->state = EVENT_STATE_REGISTERED;
                }

                break;
            }
        }

        reactor->event_queue_front = (reactor->event_queue_front + 1) % MAX_EVENTS_PER_POLL;
        reactor->pending_events--;
        reactor->total_events_processed++;
        processed++;
    }

    return processed;
}

int event_trigger_custom(EventReactor *reactor, uint32_t source_id, uint32_t flags)
{
    if (!reactor)
        return -1;

    for (uint32_t i = 0; i < reactor->event_source_count; i++)
    {
        if (reactor->event_sources[i].event_source_id == source_id)
        {
            EventSource *source = &reactor->event_sources[i];

            Event *evt = &reactor->event_queue[reactor->event_queue_rear];
            evt->event_id = next_event_id++;
            evt->source_id = source_id;
            evt->type = EVENT_TYPE_CUSTOM;
            evt->timestamp = 0;
            evt->fd = source->fd;
            evt->event_flags = flags;

            reactor->event_queue_rear = (reactor->event_queue_rear + 1) % MAX_EVENTS_PER_POLL;
            reactor->pending_events++;
            return 0;
        }
    }

    return -1;
}

int event_handle_timeout(EventReactor *reactor, uint32_t source_id)
{
    if (!reactor)
        return -1;

    for (uint32_t i = 0; i < reactor->event_source_count; i++)
    {
        if (reactor->event_sources[i].event_source_id == source_id)
        {
            EventSource *source = &reactor->event_sources[i];

            Event *evt = &reactor->event_queue[reactor->event_queue_rear];
            evt->event_id = next_event_id++;
            evt->source_id = source_id;
            evt->type = EVENT_TYPE_TIMEOUT;
            evt->timestamp = 0;
            evt->fd = source->fd;
            evt->event_flags = 0;

            reactor->event_queue_rear = (reactor->event_queue_rear + 1) % MAX_EVENTS_PER_POLL;
            reactor->pending_events++;
            return 0;
        }
    }

    return -1;
}

int event_reactor_get_metrics(EventReactor *reactor, ReactorMetrics *metrics)
{
    if (!reactor || !metrics)
        return -1;

    metrics->reactor_id = reactor->reactor_id;
    metrics->registered_sources = reactor->event_source_count;
    metrics->pending_events = reactor->pending_events;
    metrics->total_processed = reactor->total_events_processed;
    metrics->events_per_second = 1000.0f;

    return 0;
}
