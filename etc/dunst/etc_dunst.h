#ifndef ETC_DUNST_H
#define ETC_DUNST_H

#include <stdint.h>

typedef struct
{
    uint32_t config_id;
    uint32_t timeout;
    uint32_t max_notifications;
    uint32_t geometry_x;
    uint32_t geometry_y;
} etc_dunst_config_t;

typedef struct
{
    uint32_t notification_id;
    const char *summary;
    const char *body;
    uint32_t urgency;
} etc_dunst_notification_t;

int etc_dunst_init(void);
int etc_dunst_load_config(const char *filename, etc_dunst_config_t *config);
int etc_dunst_set_config(etc_dunst_config_t *config);
int etc_dunst_notify(etc_dunst_notification_t *notification);
int etc_dunst_close_notification(uint32_t notification_id);
int etc_dunst_reload(void);

#endif
