#include "notifications.h"

int notifications_init(void)
{
    return 0;
}

int notifications_create(notification_t *notif)
{
    if (!notif || !notif->title || !notif->message)
    {
        return -1;
    }
    return 0;
}

int notifications_dismiss(uint32_t id)
{
    if (id == 0)
    {
        return -1;
    }
    return 0;
}

int notifications_subscribe(void (*callback)(notification_t *))
{
    if (!callback)
    {
        return -1;
    }
    return 0;
}

int notifications_unsubscribe(void)
{
    return 0;
}
