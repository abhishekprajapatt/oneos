#include "uninstaller.h"

int uninstaller_init(void)
{
    return 0;
}

int uninstaller_register(uninstall_info_t *info)
{
    if (!info || !info->program_name || !info->uninstall_path)
    {
        return -1;
    }
    return 0;
}

int uninstaller_execute(const char *program_name)
{
    if (!program_name)
    {
        return -1;
    }
    return 0;
}

int uninstaller_clean_registry(const char *program_name)
{
    if (!program_name)
    {
        return -1;
    }
    return 0;
}

int uninstaller_remove_files(const char **file_paths, uint32_t count)
{
    if (!file_paths || count == 0)
    {
        return -1;
    }
    return 0;
}

int uninstaller_rollback(const char *program_name)
{
    if (!program_name)
    {
        return -1;
    }
    return 0;
}
