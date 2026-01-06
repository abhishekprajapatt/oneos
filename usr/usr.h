#ifndef USR_H
#define USR_H

#include <stdint.h>

int usr_init(void);
int usr_create_user(const char *username, const char *password);
int usr_delete_user(const char *username);
int usr_change_password(const char *username, const char *new_password);
int usr_list_users(char **users, uint32_t max_users);
int usr_get_user_info(const char *username, void **info);

#endif
