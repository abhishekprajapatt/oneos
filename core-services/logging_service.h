#ifndef LOGGING_SERVICE_H
#define LOGGING_SERVICE_H

#include <stdint.h>

typedef struct
{
    uint32_t log_level;
    const char *log_message;
    uint64_t timestamp;
    const char *source_module;
} log_entry_t;

int logging_service_init(void);
int logging_service_log_message(uint32_t level, const char *message);
int logging_service_set_log_level(uint32_t level);
int logging_service_write_to_file(const char *filename);
int logging_service_rotate_logs(void);
int logging_service_get_logs(log_entry_t *logs, uint32_t *count, uint32_t limit);
int logging_service_clear_logs(void);

#endif
