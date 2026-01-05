#ifndef MAN_MAN3_H
#define MAN_MAN3_H

#include <stdint.h>

typedef struct
{
    uint32_t lib_func_id;
    const char *function_name;
    const char *library_name;
    const char *prototype;
    const char *description;
} man_man3_entry_t;

typedef struct
{
    uint32_t total_functions;
    uint32_t documented_functions;
    uint32_t library_count;
} man_man3_state_t;

int man_man3_init(void);
int man_man3_add_function(man_man3_entry_t *entry);
int man_man3_remove_function(uint32_t func_id);
int man_man3_get_function(const char *name, man_man3_entry_t *entry);
int man_man3_get_library_functions(const char *lib_name, man_man3_entry_t *results, uint32_t *count);

#endif
