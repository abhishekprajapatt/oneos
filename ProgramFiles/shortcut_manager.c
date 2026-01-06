#include "shortcut_manager.h"

int shortcut_manager_init(void)
{
    return 0;
}

int shortcut_manager_create(shortcut_t *shortcut)
{
    if (!shortcut || !shortcut->shortcut_name || !shortcut->target_path)
    {
        return -1;
    }
    return 0;
}

int shortcut_manager_delete(const char *shortcut_name)
{
    if (!shortcut_name)
    {
        return -1;
    }
    return 0;
}

int shortcut_manager_get(const char *shortcut_name, shortcut_t *shortcut)
{
    if (!shortcut_name || !shortcut)
    {
        return -1;
    }
    return 0;
}

int shortcut_manager_list(char **shortcuts, uint32_t max_shortcuts)
{
    if (!shortcuts || max_shortcuts == 0)
    {
        return -1;
    }
    return 0;
}

int shortcut_manager_resolve(const char *shortcut_path, char *target, uint32_t max_len)
{
    if (!shortcut_path || !target || max_len == 0)
    {
        return -1;
    }
    return 0;
}
