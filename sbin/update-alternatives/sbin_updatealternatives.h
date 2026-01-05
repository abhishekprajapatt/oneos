#ifndef SBIN_UPDATEALTERNATIVES_H
#define SBIN_UPDATEALTERNATIVES_H

#include <stdint.h>

typedef struct
{
    uint32_t link_id;
    const char *link_name;
    const char *link_path;
    uint32_t priority;
    uint32_t link_status;
} sbin_updatealternatives_entry_t;

typedef struct
{
    uint32_t total_links;
    uint32_t configured_links;
    uint32_t auto_links;
    uint32_t manual_links;
} sbin_updatealternatives_state_t;

int sbin_updatealternatives_init(void);
int sbin_updatealternatives_install(sbin_updatealternatives_entry_t *entry);
int sbin_updatealternatives_remove(const char *link_name);
int sbin_updatealternatives_get_status(const char *link_name, sbin_updatealternatives_entry_t *entry);
int sbin_updatealternatives_get_state(sbin_updatealternatives_state_t *state);

#endif
