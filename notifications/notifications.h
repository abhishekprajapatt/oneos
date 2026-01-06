#ifndef NOTIFICATIONS_H
#define NOTIFICATIONS_H

#include <stdint.h>

typedef struct
{
    uint32_t id;
    const char *title;
    const char *message;
} notification_t;

int notifications_init(void);
int notifications_create(notification_t *notif);
int notifications_dismiss(uint32_t id);
int notifications_subscribe(void (*callback)(notification_t *));
int notifications_unsubscribe(void);

#endif
