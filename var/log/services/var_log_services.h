#ifndef VAR_LOG_SERVICES_H
#define VAR_LOG_SERVICES_H

#include <stdint.h>

typedef struct
{
    uint32_t service_log_id;
    const char *service_name;
    const char *log_level;
    const char *log_message;
    uint64_t log_timestamp;
} var_log_services_entry_t;

typedef struct
{
    uint32_t total_service_logs;
    uint32_t error_logs;
    uint32_t warning_logs;
    uint64_t total_size;
} var_log_services_state_t;

int var_log_services_init(void);
int var_log_services_list(var_log_services_entry_t *entries, uint32_t *count);
int var_log_services_get_service_logs(const char *service_name, var_log_services_entry_t *entries, uint32_t *count);
int var_log_services_write_log(const char *service_name, const char *level, const char *message);
int var_log_services_get_latest(uint32_t count, var_log_services_entry_t *entries);
int var_log_services_get_state(var_log_services_state_t *state);

#endif
