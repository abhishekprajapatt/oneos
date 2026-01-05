#ifndef NODE_H
#define NODE_H

#include <stdint.h>

typedef struct
{
    const char *script_file;
    char **arguments;
    uint32_t arg_count;
    uint32_t debug_mode;
    uint32_t max_memory;
} node_execution_options_t;

typedef struct
{
    const char *node_version;
    const char *v8_version;
    uint64_t heap_total;
    uint64_t heap_used;
    uint64_t external;
} node_runtime_info_t;

int node_init(void);
int node_execute_script(const char *script_file);
int node_execute_with_options(node_execution_options_t *options);
int node_install_package(const char *package_name);
int node_get_runtime_info(node_runtime_info_t *info);
int node_repl_start(void);

#endif
