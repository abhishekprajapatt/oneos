#ifndef SECURITY_CORE_H
#define SECURITY_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t permission_id;
    const char *permission_name;
    uint32_t permission_mask;
    uint32_t permission_type;
} security_core_permission_t;

typedef struct
{
    uint32_t total_permissions;
    uint32_t granted_permissions;
    uint32_t denied_permissions;
    uint32_t audit_events;
} security_core_state_t;

int security_core_init(void);
int security_core_grant_permission(security_core_permission_t *perm);
int security_core_revoke_permission(uint32_t permission_id);
int security_core_check_permission(uint32_t user_id, uint32_t permission_id);
int security_core_audit_access(const char *event);
int security_core_get_state(security_core_state_t *state);

#endif
