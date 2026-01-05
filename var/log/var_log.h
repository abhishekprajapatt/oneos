#ifndef VAR_LOG_H
#define VAR_LOG_H

#include <stdint.h>

typedef struct
{
    uint32_t log_id;
    const char *log_name;
    const char *log_level;
    const char *log_path;
    uint64_t log_size;
} var_log_entry_t;

typedef struct
{
    uint32_t total_logs;
    uint32_t critical_logs;
    uint32_t warning_logs;
    uint64_t total_log_size;
} var_log_state_t;

int var_log_init(void);
int var_log_list(var_log_entry_t *logs, uint32_t *count);
int var_log_get(const char *log_name, var_log_entry_t *log);
int var_log_write(const char *log_name, const char *message, uint32_t level);
int var_log_rotate(const char *log_name);
int var_log_clear(const char *log_name);
int var_log_get_state(var_log_state_t *state);

#endif
