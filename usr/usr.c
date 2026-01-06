#include "usr.h"

int usr_init(void)
{
    return 0;
}

int usr_create_user(const char *username, const char *password)
{
    if (!username || !password)
    {
        return -1;
    }
    return 0;
}

int usr_delete_user(const char *username)
{
    if (!username)
    {
        return -1;
    }
    return 0;
}

int usr_change_password(const char *username, const char *new_password)
{
    if (!username || !new_password)
    {
        return -1;
    }
    return 0;
}

int usr_list_users(char **users, uint32_t max_users)
{
    if (!users || max_users == 0)
    {
        return -1;
    }
    return 0;
}

int usr_get_user_info(const char *username, void **info)
{
    if (!username || !info)
    {
        return -1;
    }
    return 0;
}
