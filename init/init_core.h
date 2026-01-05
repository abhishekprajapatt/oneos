#ifndef INIT_CORE_H
#define INIT_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t process_id;
    const char *process_name;
    uint32_t runlevel;
    uint32_t status;
} init_core_process_t;

typedef struct
{
    uint32_t runlevel;
    uint32_t process_count;
    uint32_t active_count;
} init_core_state_t;

int init_core_start(void);
int init_core_shutdown(void);
int init_core_set_runlevel(uint32_t runlevel);
int init_core_get_runlevel(uint32_t *runlevel);
int init_core_register_process(init_core_process_t *process);
int init_core_unregister_process(uint32_t process_id);

#endif
