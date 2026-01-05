#ifndef VAR_LOG_APPLICATIONS_H
#define VAR_LOG_APPLICATIONS_H

#include <stdint.h>

typedef struct
{
    uint32_t app_log_id;
    const char *app_name;
    const char *log_level;
    const char *log_message;
    uint64_t log_timestamp;
} var_log_applications_entry_t;

typedef struct
{
    uint32_t total_log_entries;
    uint32_t error_entries;
    uint32_t warning_entries;
    uint64_t total_log_size;
} var_log_applications_state_t;

int var_log_applications_init(void);
int var_log_applications_list(var_log_applications_entry_t *entries, uint32_t *count);
int var_log_applications_get_app_logs(const char *app_name, var_log_applications_entry_t *entries, uint32_t *count);
int var_log_applications_write_log(const char *app_name, const char *level, const char *message);
int var_log_applications_filter_by_level(const char *level, var_log_applications_entry_t *entries, uint32_t *count);
int var_log_applications_get_state(var_log_applications_state_t *state);

#endif
