#include "security_manager.h"
#include <stdlib.h>
#include <string.h>

SecurityManager *security_manager_init(void)
{
    SecurityManager *sm = (SecurityManager *)malloc(sizeof(SecurityManager));
    if (!sm)
        return NULL;

    sm->user_count = 0;
    sm->group_count = 0;
    sm->permission_count = 0;
    sm->current_user_uid = 0;

    sm->groups = (Group *)malloc(MAX_GROUPS * sizeof(Group));
    if (!sm->groups)
    {
        free(sm);
        return NULL;
    }

    memset(sm->users, 0, sizeof(sm->users));
    memset(sm->groups, 0, MAX_GROUPS * sizeof(Group));
    memset(sm->permissions, 0, sizeof(sm->permissions));

    return sm;
}

void security_manager_cleanup(SecurityManager *sm)
{
    if (!sm)
        return;
    if (sm->groups)
    {
        for (uint32_t i = 0; i < sm->group_count; i++)
        {
            if (sm->groups[i].member_uids)
            {
                free(sm->groups[i].member_uids);
            }
        }
        free(sm->groups);
    }
    free(sm);
}

uint32_t security_add_user(SecurityManager *sm, const char *username, const char *password)
{
    if (!sm || sm->user_count >= MAX_USERS || !username || !password)
        return 0;

    User *user = &sm->users[sm->user_count];
    user->uid = sm->user_count + 1;
    strncpy(user->username, username, 255);
    strncpy(user->password_hash, password, 255);
    user->gid = 0;
    user->is_admin = false;
    user->is_active = true;

    sm->user_count++;
    return user->uid;
}

int security_remove_user(SecurityManager *sm, uint32_t uid)
{
    if (!sm)
        return -1;

    for (uint32_t i = 0; i < sm->user_count; i++)
    {
        if (sm->users[i].uid == uid)
        {
            for (uint32_t j = i; j < sm->user_count - 1; j++)
            {
                sm->users[j] = sm->users[j + 1];
            }
            sm->user_count--;
            return 0;
        }
    }

    return -1;
}

bool security_authenticate_user(SecurityManager *sm, const char *username, const char *password)
{
    if (!sm || !username || !password)
        return false;

    for (uint32_t i = 0; i < sm->user_count; i++)
    {
        if (strcmp(sm->users[i].username, username) == 0)
        {
            if (strcmp(sm->users[i].password_hash, password) == 0 && sm->users[i].is_active)
            {
                sm->current_user_uid = sm->users[i].uid;
                return true;
            }
        }
    }

    return false;
}

int security_set_permissions(SecurityManager *sm, uint32_t resource_id, uint32_t owner_uid,
                             Permission owner, Permission group, Permission other)
{
    if (!sm || sm->permission_count >= MAX_PERMISSIONS)
        return -1;

    PermissionEntry *perm = &sm->permissions[sm->permission_count];
    perm->resource_id = resource_id;
    perm->owner_uid = owner_uid;
    perm->owner_perms = owner;
    perm->group_perms = group;
    perm->other_perms = other;

    sm->permission_count++;
    return 0;
}

bool security_check_permission(SecurityManager *sm, uint32_t resource_id, uint32_t user_uid, Permission required)
{
    if (!sm)
        return false;

    for (uint32_t i = 0; i < sm->permission_count; i++)
    {
        if (sm->permissions[i].resource_id == resource_id)
        {
            if (sm->permissions[i].owner_uid == user_uid)
            {
                return (sm->permissions[i].owner_perms & required) == required;
            }
            return (sm->permissions[i].other_perms & required) == required;
        }
    }

    return false;
}

int security_set_admin_privilege(SecurityManager *sm, uint32_t uid, bool is_admin)
{
    if (!sm)
        return -1;

    for (uint32_t i = 0; i < sm->user_count; i++)
    {
        if (sm->users[i].uid == uid)
        {
            sm->users[i].is_admin = is_admin;
            return 0;
        }
    }

    return -1;
}
