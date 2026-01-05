#ifndef SECURITY_SELINUX_H
#define SECURITY_SELINUX_H

#include <stdint.h>

typedef struct
{
    uint32_t context_id;
    const char *user_context;
    const char *role_context;
    const char *type_context;
    uint32_t context_status;
} security_selinux_context_t;

typedef struct
{
    uint32_t total_contexts;
    uint32_t active_contexts;
    uint32_t policy_enforced;
    uint32_t policy_updates;
} security_selinux_state_t;

int security_selinux_init(void);
int security_selinux_set_context(security_selinux_context_t *context);
int security_selinux_get_context(uint32_t context_id, security_selinux_context_t *context);
int security_selinux_check_access(const char *source, const char *target, const char *class);
int security_selinux_load_policy(const char *policy_path);
int security_selinux_get_state(security_selinux_state_t *state);

#endif
