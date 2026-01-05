#ifndef CALENDAR_H
#define CALENDAR_H

#include <stdint.h>

typedef struct
{
    uint32_t event_id;
    const char *event_title;
    const char *event_description;
    uint32_t start_time;
    uint32_t end_time;
    const char *location;
} calendar_event_t;

typedef struct
{
    uint32_t year;
    uint32_t month;
    uint32_t day;
} calendar_date_t;

int calendar_init(void);
int calendar_create_event(calendar_event_t *event);
int calendar_delete_event(uint32_t event_id);
int calendar_update_event(calendar_event_t *event);
int calendar_list_events(calendar_date_t date, calendar_event_t *events, uint32_t *count);
int calendar_get_event(uint32_t event_id, calendar_event_t *event);
int calendar_set_reminder(uint32_t event_id, uint32_t reminder_minutes);
int calendar_get_events_for_month(uint32_t year, uint32_t month, calendar_event_t *events, uint32_t *count);

#endif
