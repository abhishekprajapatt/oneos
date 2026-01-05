#ifndef PYTHON_M_H
#define PYTHON_M_H

#include <stdint.h>

typedef struct
{
    const char *module_name;
    char **arguments;
    uint32_t arg_count;
    uint32_t search_path;
} python_m_execution_options_t;

typedef struct
{
    uint32_t modules_found;
    uint32_t modules_loaded;
    uint64_t execution_time;
} python_m_stats_t;

int python_m_init(void);
int python_m_execute_module(const char *module_name);
int python_m_execute_with_options(python_m_execution_options_t *options);
int python_m_list_modules(char *module_list, uint32_t list_size);
int python_m_get_module_path(const char *module_name, char *path, uint32_t path_size);
int python_m_get_stats(python_m_stats_t *stats);

#endif
