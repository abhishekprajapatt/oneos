#ifndef SYSTEM_PREFERENCES_H
#define SYSTEM_PREFERENCES_H

#include <stdint.h>

typedef struct
{
    uint32_t pref_id;
    const char *pref_name;
    const char *pref_key;
    const char *pref_value;
    uint32_t pref_type;
} system_preferences_entry_t;

typedef struct
{
    uint32_t total_preferences;
    uint32_t modified_preferences;
    uint32_t default_preferences;
} system_preferences_state_t;

int system_preferences_init(void);
int system_preferences_set(system_preferences_entry_t *preference);
int system_preferences_get(const char *pref_key, system_preferences_entry_t *preference);
int system_preferences_delete(uint32_t pref_id);
int system_preferences_reset_defaults(void);
int system_preferences_save(void);
int system_preferences_get_state(system_preferences_state_t *state);

#endif
