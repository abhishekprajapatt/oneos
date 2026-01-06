#include "preference_panes.h"

int preference_panes_init(void)
{
    return 0;
}

int preference_panes_register(preference_pane_t *pane)
{
    if (!pane || !pane->name)
    {
        return -1;
    }
    return 0;
}

int preference_panes_unregister(const char *pane_name)
{
    if (!pane_name)
    {
        return -1;
    }
    return 0;
}

int preference_panes_open(const char *pane_name)
{
    if (!pane_name)
    {
        return -1;
    }
    return 0;
}

int preference_panes_list(preference_pane_t **panes, uint32_t *count)
{
    if (!panes || !count)
    {
        return -1;
    }
    *count = 0;
    return 0;
}

int preference_panes_get_pane(const char *pane_name, preference_pane_t *pane)
{
    if (!pane_name || !pane)
    {
        return -1;
    }
    return 0;
}
