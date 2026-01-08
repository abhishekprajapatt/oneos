#ifndef SECURITY_MANAGER_H
#define SECURITY_MANAGER_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_USERS 1024
#define MAX_GROUPS 512
#define MAX_PERMISSIONS 2048

typedef enum
{
    PERM_EXECUTE = 0x01,
    PERM_WRITE = 0x02,
    PERM_READ = 0x04,
    PERM_DELETE = 0x08,
    PERM_MODIFY = 0x10
} Permission;

typedef struct
{
    uint32_t uid;
    char username[256];
    char password_hash[256];
    uint32_t gid;
    bool is_admin;
    bool is_active;
} User;

typedef struct
{
    uint32_t gid;
    char groupname[256];
    uint32_t *member_uids;
    uint32_t member_count;
} Group;

typedef struct
{
    uint32_t resource_id;
    uint32_t owner_uid;
    uint32_t owner_gid;
    Permission owner_perms;
    Permission group_perms;
    Permission other_perms;
} PermissionEntry;

typedef struct
{
    User users[MAX_USERS];
    uint32_t user_count;
    Group *groups;
    uint32_t group_count;
    PermissionEntry permissions[MAX_PERMISSIONS];
    uint32_t permission_count;
    uint32_t current_user_uid;
} SecurityManager;

SecurityManager *security_manager_init(void);
void security_manager_cleanup(SecurityManager *sm);
uint32_t security_add_user(SecurityManager *sm, const char *username, const char *password);
int security_remove_user(SecurityManager *sm, uint32_t uid);
bool security_authenticate_user(SecurityManager *sm, const char *username, const char *password);
int security_set_permissions(SecurityManager *sm, uint32_t resource_id, uint32_t owner_uid, Permission owner, Permission group, Permission other);
bool security_check_permission(SecurityManager *sm, uint32_t resource_id, uint32_t user_uid, Permission required);
int security_set_admin_privilege(SecurityManager *sm, uint32_t uid, bool is_admin);

#endif
