#include "program_manager.h"

int program_manager_init(void)
{
    return 0;
}

int program_manager_add_program(program_info_t *program)
{
    if (!program || !program->program_name || !program->version)
    {
        return -1;
    }
    return 0;
}

int program_manager_remove_program(const char *program_name)
{
    if (!program_name)
    {
        return -1;
    }
    return 0;
}

int program_manager_get_program(const char *program_name, program_info_t *info)
{
    if (!program_name || !info)
    {
        return -1;
    }
    return 0;
}

int program_manager_list_programs(program_info_t **programs, uint32_t *count)
{
    if (!programs || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int program_manager_update_program(const char *program_name, const char *new_version)
{
    if (!program_name || !new_version)
    {
        return -1;
    }
    return 0;
}
