#ifndef VAR_CRASH_H
#define VAR_CRASH_H

#include <stdint.h>

typedef struct
{
    uint32_t crash_id;
    const char *crash_name;
    const char *crash_type;
    const char *crash_path;
    uint64_t crash_size;
    uint64_t crash_time;
} var_crash_entry_t;

typedef struct
{
    uint32_t total_crashes;
    uint32_t analyzed_crashes;
    uint32_t unresolved_crashes;
    uint64_t total_crash_data_size;
} var_crash_state_t;

int var_crash_init(void);
int var_crash_list(var_crash_entry_t *crashes, uint32_t *count);
int var_crash_get_dump(const char *crash_name, var_crash_entry_t *crash);
int var_crash_analyze(const char *crash_name, char *report, uint32_t *report_size);
int var_crash_remove(const char *crash_name);
int var_crash_get_state(var_crash_state_t *state);

#endif
