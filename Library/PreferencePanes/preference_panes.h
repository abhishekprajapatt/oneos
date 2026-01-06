#ifndef PREFERENCE_PANES_H
#define PREFERENCE_PANES_H

#include <stdint.h>

typedef struct
{
    const char *name;
    const char *icon;
    void (*open_pane)(void);
} preference_pane_t;

int preference_panes_init(void);
int preference_panes_register(preference_pane_t *pane);
int preference_panes_unregister(const char *pane_name);
int preference_panes_open(const char *pane_name);
int preference_panes_list(preference_pane_t **panes, uint32_t *count);
int preference_panes_get_pane(const char *pane_name, preference_pane_t *pane);

#endif
