#ifndef MAIL_H
#define MAIL_H

#include <stdint.h>

typedef struct
{
    uint32_t email_id;
    const char *sender;
    const char *recipient;
    const char *subject;
    const char *body;
    uint32_t timestamp;
    uint32_t is_read;
} email_message_t;

typedef struct
{
    uint32_t total_emails;
    uint32_t unread_emails;
    uint32_t drafts;
    uint32_t sent;
} mail_stats_t;

int mail_init(void);
int mail_send(email_message_t *message);
int mail_get_inbox(email_message_t *emails, uint32_t *count);
int mail_get_email(uint32_t email_id, email_message_t *message);
int mail_mark_as_read(uint32_t email_id);
int mail_delete_email(uint32_t email_id);
int mail_create_draft(email_message_t *message);
int mail_get_stats(mail_stats_t *stats);

#endif
