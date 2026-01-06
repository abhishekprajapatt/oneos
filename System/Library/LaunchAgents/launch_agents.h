#ifndef LAUNCH_AGENTS_H
#define LAUNCH_AGENTS_H

#include <stdint.h>

typedef struct
{
    const char *label;
    const char *program;
    uint32_t status;
} launch_agent_t;

int launch_agents_init(void);
int launch_agents_load(const char *plist_path);
int launch_agents_unload(const char *agent_label);
int launch_agents_start(const char *agent_label);
int launch_agents_stop(const char *agent_label);
int launch_agents_list(launch_agent_t **agents, uint32_t *count);

#endif
