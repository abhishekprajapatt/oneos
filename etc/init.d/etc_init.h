#ifndef ETC_INIT_H
#define ETC_INIT_H

#include <stdint.h>

typedef struct
{
    uint32_t script_id;
    const char *script_name;
    uint32_t runlevel;
    uint32_t priority;
} etc_init_script_t;

typedef struct
{
    uint32_t runlevel;
    uint32_t script_count;
    uint32_t status;
} etc_init_state_t;

int etc_init_register_script(etc_init_script_t *script);
int etc_init_unregister_script(uint32_t script_id);
int etc_init_execute_script(uint32_t script_id, const char *action);
int etc_init_set_runlevel(uint32_t runlevel);
int etc_init_get_runlevel(uint32_t *runlevel);
int etc_init_list_scripts(uint32_t runlevel, etc_init_script_t *scripts, uint32_t max_count);

#endif
