#ifndef VAR_SPOOL_MAIL_H
#define VAR_SPOOL_MAIL_H

#include <stdint.h>

typedef struct
{
    uint32_t mail_id;
    const char *mail_user;
    const char *mail_subject;
    const char *mail_path;
    uint64_t mail_size;
} var_spool_mail_entry_t;

typedef struct
{
    uint32_t total_mail_items;
    uint32_t unread_mail;
    uint32_t read_mail;
    uint64_t total_mail_size;
} var_spool_mail_state_t;

int var_spool_mail_init(void);
int var_spool_mail_list(var_spool_mail_entry_t *mails, uint32_t *count);
int var_spool_mail_get_user_mail(const char *user, var_spool_mail_entry_t *mails, uint32_t *count);
int var_spool_mail_store(const char *user, const char *subject, const void *data, uint64_t size);
int var_spool_mail_delete(const char *user, uint32_t mail_id);
int var_spool_mail_get_state(var_spool_mail_state_t *state);

#endif
