#ifndef TOOLS_SCRIPTS_H
#define TOOLS_SCRIPTS_H

#include <stdint.h>

typedef struct
{
    uint32_t script_id;
    const char *script_name;
    const char *script_path;
    const char *script_type;
    uint32_t execution_status;
} tools_scripts_entry_t;

typedef struct
{
    uint32_t total_scripts;
    uint32_t running_scripts;
    uint32_t completed_scripts;
    uint32_t failed_scripts;
} tools_scripts_state_t;

int tools_scripts_init(void);
int tools_scripts_register(tools_scripts_entry_t *script);
int tools_scripts_unregister(uint32_t script_id);
int tools_scripts_execute(uint32_t script_id, const char *args);
int tools_scripts_stop(uint32_t script_id);
int tools_scripts_get_status(uint32_t script_id, uint32_t *status);
int tools_scripts_get_state(tools_scripts_state_t *state);

#endif
