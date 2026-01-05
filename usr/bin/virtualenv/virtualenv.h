#ifndef VIRTUALENV_H
#define VIRTUALENV_H

#include <stdint.h>

typedef struct
{
    const char *env_path;
    const char *python_executable;
    uint32_t system_site_packages;
    uint32_t no_pip;
} virtualenv_create_options_t;

typedef struct
{
    uint32_t total_envs;
    uint32_t active_env;
    uint64_t total_size;
} virtualenv_stats_t;

int virtualenv_init(void);
int virtualenv_create_env(const char *env_path);
int virtualenv_create_with_options(virtualenv_create_options_t *options);
int virtualenv_activate_env(const char *env_path);
int virtualenv_deactivate_env(void);
int virtualenv_delete_env(const char *env_path);
int virtualenv_get_stats(virtualenv_stats_t *stats);

#endif
