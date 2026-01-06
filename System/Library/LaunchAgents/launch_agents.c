#include "launch_agents.h"

int launch_agents_init(void)
{
    return 0;
}

int launch_agents_load(const char *plist_path)
{
    if (!plist_path)
    {
        return -1;
    }
    return 0;
}

int launch_agents_unload(const char *agent_label)
{
    if (!agent_label)
    {
        return -1;
    }
    return 0;
}

int launch_agents_start(const char *agent_label)
{
    if (!agent_label)
    {
        return -1;
    }
    return 0;
}

int launch_agents_stop(const char *agent_label)
{
    if (!agent_label)
    {
        return -1;
    }
    return 0;
}

int launch_agents_list(launch_agent_t **agents, uint32_t *count)
{
    if (!agents || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}
