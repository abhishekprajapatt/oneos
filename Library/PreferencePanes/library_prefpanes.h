#ifndef LIBRARY_PREFPANES_H
#define LIBRARY_PREFPANES_H

#include <stdint.h>

typedef struct
{
    uint32_t pane_id;
    const char *pane_name;
    const char *pane_icon;
    const char *pane_bundle;
    uint32_t pane_type;
} library_prefpanes_entry_t;

typedef struct
{
    uint32_t total_panes;
    uint32_t enabled_panes;
    uint32_t disabled_panes;
    uint32_t system_panes;
} library_prefpanes_state_t;

int library_prefpanes_init(void);
int library_prefpanes_register(library_prefpanes_entry_t *entry);
int library_prefpanes_unregister(uint32_t pane_id);
int library_prefpanes_get_pane(const char *name, library_prefpanes_entry_t *entry);
int library_prefpanes_enable(uint32_t pane_id);
int library_prefpanes_disable(uint32_t pane_id);

#endif
