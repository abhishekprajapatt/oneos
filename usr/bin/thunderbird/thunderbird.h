#ifndef THUNDERBIRD_H
#define THUNDERBIRD_H

#include <stdint.h>

typedef struct
{
    const char *email_address;
    const char *password;
    const char *imap_server;
    const char *smtp_server;
} thunderbird_account_options_t;

typedef struct
{
    uint32_t total_accounts;
    uint32_t unread_messages;
    uint32_t total_messages;
    uint64_t mailbox_size;
} thunderbird_mailbox_stats_t;

int thunderbird_init(void);
int thunderbird_launch(void);
int thunderbird_add_account(thunderbird_account_options_t *account);
int thunderbird_sync_email(const char *account_name);
int thunderbird_get_mailbox_stats(thunderbird_mailbox_stats_t *stats);
int thunderbird_compose_message(const char *to, const char *subject, const char *body);

#endif
