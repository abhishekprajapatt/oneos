#ifndef VAR_LOG_AUDIT_H
#define VAR_LOG_AUDIT_H

#include <stdint.h>

typedef struct
{
    uint32_t audit_id;
    uint64_t audit_timestamp;
    const char *audit_event;
    const char *audit_user;
    const char *audit_result;
} var_log_audit_entry_t;

typedef struct
{
    uint32_t total_audit_logs;
    uint32_t success_entries;
    uint32_t failure_entries;
    uint64_t total_audit_size;
} var_log_audit_state_t;

int var_log_audit_init(void);
int var_log_audit_list(var_log_audit_entry_t *entries, uint32_t *count);
int var_log_audit_get_by_user(const char *user, var_log_audit_entry_t *entries, uint32_t *count);
int var_log_audit_get_by_event(const char *event, var_log_audit_entry_t *entries, uint32_t *count);
int var_log_audit_write_event(const char *event, const char *user, const char *result);
int var_log_audit_get_state(var_log_audit_state_t *state);

#endif
