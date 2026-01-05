#ifndef ONEOS_CORE_H
#define ONEOS_CORE_H

#include <stdint.h>

typedef struct
{
    uint32_t core_id;
    const char *core_name;
    const char *version;
} oneos_core_config_t;

int oneos_core_init(void);
int oneos_core_initialize_memory(void);
int oneos_core_setup_interrupts(void);
int oneos_core_load_modules(void);
int oneos_core_start_scheduler(void);
int oneos_core_enable_memory_management(void);

#endif
