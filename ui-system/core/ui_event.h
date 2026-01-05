#ifndef UI_EVENT_H
#define UI_EVENT_H

#include <stdint.h>

typedef struct
{
    uint32_t event_id;
    uint32_t event_type;
    uint32_t source_widget_id;
    uint32_t timestamp;
    const char *event_data;
} ui_event_t;

typedef struct
{
    uint32_t total_events;
    uint32_t pending_events;
    uint32_t processed_events;
    uint64_t event_queue_size;
} ui_event_stats_t;

int ui_event_init(void);
int ui_event_post(ui_event_t *event);
int ui_event_poll(ui_event_t *event);
int ui_event_dispatch(uint32_t widget_id);
int ui_event_register_handler(uint32_t widget_id, uint32_t event_type, void *handler);
int ui_event_unregister_handler(uint32_t widget_id, uint32_t event_type);
int ui_event_clear_queue(void);
int ui_event_get_stats(ui_event_stats_t *stats);

#endif
