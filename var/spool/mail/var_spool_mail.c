#include "var_spool_mail.h"

int var_spool_mail_init(void)
{
    return 0;
}

int var_spool_mail_list(var_spool_mail_entry_t *mails, uint32_t *count)
{
    if (!mails || !count)
    {
        return -1;
    }
    return 0;
}

int var_spool_mail_get_user_mail(const char *user, var_spool_mail_entry_t *mails, uint32_t *count)
{
    if (!user || !mails || !count)
    {
        return -1;
    }
    return 0;
}

int var_spool_mail_store(const char *user, const char *subject, const void *data, uint64_t size)
{
    if (!user || !subject || !data)
    {
        return -1;
    }
    return 0;
}

int var_spool_mail_delete(const char *user, uint32_t mail_id)
{
    if (!user)
    {
        return -1;
    }
    return 0;
}

int var_spool_mail_get_state(var_spool_mail_state_t *state)
{
    if (!state)
    {
        return -1;
    }
    return 0;
}
