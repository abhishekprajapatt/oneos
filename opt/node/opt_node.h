#ifndef OPT_NODE_H
#define OPT_NODE_H

#include <stdint.h>

typedef struct
{
    uint32_t app_id;
    const char *app_name;
    const char *version;
    const char *entry_point;
    uint32_t status;
} opt_node_app_t;

typedef struct
{
    uint32_t total_apps;
    uint32_t running_apps;
    uint32_t stopped_apps;
    uint64_t total_memory_usage;
} opt_node_state_t;

int opt_node_init(void);
int opt_node_register_app(opt_node_app_t *app);
int opt_node_unregister_app(uint32_t app_id);
int opt_node_start_app(uint32_t app_id);
int opt_node_stop_app(uint32_t app_id);
int opt_node_get_app_info(uint32_t app_id, opt_node_app_t *app);
int opt_node_get_state(opt_node_state_t *state);

#endif
