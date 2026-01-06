#ifndef PROGRAM_MANAGER_H
#define PROGRAM_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *program_name;
    const char *version;
    uint32_t install_date;
    uint32_t size;
} program_info_t;

int program_manager_init(void);
int program_manager_add_program(program_info_t *program);
int program_manager_remove_program(const char *program_name);
int program_manager_get_program(const char *program_name, program_info_t *info);
int program_manager_list_programs(program_info_t **programs, uint32_t *count);
int program_manager_update_program(const char *program_name, const char *new_version);

#endif
