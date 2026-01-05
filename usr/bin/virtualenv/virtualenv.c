#include "virtualenv.h"

int virtualenv_init(void)
{
    return 0;
}

int virtualenv_create_env(const char *env_path)
{
    if (!env_path)
    {
        return -1;
    }
    return 0;
}

int virtualenv_create_with_options(virtualenv_create_options_t *options)
{
    if (!options || !options->env_path)
    {
        return -1;
    }
    return 0;
}

int virtualenv_activate_env(const char *env_path)
{
    if (!env_path)
    {
        return -1;
    }
    return 0;
}

int virtualenv_deactivate_env(void)
{
    return 0;
}

int virtualenv_delete_env(const char *env_path)
{
    if (!env_path)
    {
        return -1;
    }
    return 0;
}

int virtualenv_get_stats(virtualenv_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}
