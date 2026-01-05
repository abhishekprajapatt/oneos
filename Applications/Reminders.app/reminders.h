#ifndef REMINDERS_H
#define REMINDERS_H

#include <stdint.h>

typedef struct
{
    uint32_t reminder_id;
    const char *reminder_title;
    const char *reminder_description;
    uint32_t due_time;
    const char *priority;
    uint32_t is_completed;
} reminder_item_t;

typedef struct
{
    uint32_t total_reminders;
    uint32_t completed_reminders;
    uint32_t pending_reminders;
    uint32_t overdue_reminders;
} reminders_stats_t;

int reminders_init(void);
int reminders_create(reminder_item_t *reminder);
int reminders_delete(uint32_t reminder_id);
int reminders_update(reminder_item_t *reminder);
int reminders_get(uint32_t reminder_id, reminder_item_t *reminder);
int reminders_list_all(reminder_item_t *reminders, uint32_t *count);
int reminders_mark_complete(uint32_t reminder_id);
int reminders_get_stats(reminders_stats_t *stats);

#endif
