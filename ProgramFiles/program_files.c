#include "program_files.h"

int program_files_init(void)
{
    return 0;
}

int program_files_install(const char *program_name, const char *path)
{
    if (!program_name || !path)
    {
        return -1;
    }
    return 0;
}

int program_files_uninstall(const char *program_name)
{
    if (!program_name)
    {
        return -1;
    }
    return 0;
}

int program_files_list(char **programs, uint32_t max_programs)
{
    if (!programs || max_programs == 0)
    {
        return -1;
    }
    return 0;
}

int program_files_get_path(const char *program_name, char *path, uint32_t max_len)
{
    if (!program_name || !path || max_len == 0)
    {
        return -1;
    }
    return 0;
}
