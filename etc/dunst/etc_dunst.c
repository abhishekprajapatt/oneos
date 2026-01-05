#include "etc_dunst.h"

int etc_dunst_init(void)
{
    return 0;
}

int etc_dunst_load_config(const char *filename, etc_dunst_config_t *config)
{
    if (!filename || !config)
    {
        return -1;
    }
    return 0;
}

int etc_dunst_set_config(etc_dunst_config_t *config)
{
    if (!config)
    {
        return -1;
    }
    return 0;
}

int etc_dunst_notify(etc_dunst_notification_t *notification)
{
    if (!notification)
    {
        return -1;
    }
    return 0;
}

int etc_dunst_close_notification(uint32_t notification_id)
{
    return 0;
}

int etc_dunst_reload(void)
{
    return 0;
}
