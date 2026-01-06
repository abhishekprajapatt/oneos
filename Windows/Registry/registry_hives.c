#include "registry_hives.h"

int registry_hives_init_hklm(void)
{
    return 0;
}

int registry_hives_init_hkcu(void)
{
    return 0;
}

int registry_hives_init_hkcr(void)
{
    return 0;
}

int registry_hives_init_hku(void)
{
    return 0;
}

int registry_hives_init_hkcc(void)
{
    return 0;
}

int registry_hives_backup_all(const char *backup_dir)
{
    if (!backup_dir)
    {
        return -1;
    }
    return 0;
}

int registry_hives_restore_all(const char *backup_dir)
{
    if (!backup_dir)
    {
        return -1;
    }
    return 0;
}
