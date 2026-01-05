#include "mail.h"

int mail_init(void)
{
    return 0;
}

int mail_send(email_message_t *message)
{
    if (!message)
    {
        return -1;
    }
    return 0;
}

int mail_get_inbox(email_message_t *emails, uint32_t *count)
{
    if (!emails || !count)
    {
        return -1;
    }
    return 0;
}

int mail_get_email(uint32_t email_id, email_message_t *message)
{
    if (email_id == 0 || !message)
    {
        return -1;
    }
    return 0;
}

int mail_mark_as_read(uint32_t email_id)
{
    if (email_id == 0)
    {
        return -1;
    }
    return 0;
}

int mail_delete_email(uint32_t email_id)
{
    if (email_id == 0)
    {
        return -1;
    }
    return 0;
}

int mail_create_draft(email_message_t *message)
{
    if (!message)
    {
        return -1;
    }
    return 0;
}

int mail_get_stats(mail_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
