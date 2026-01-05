#ifndef LIBRARY_APPSUPPORT_H
#define LIBRARY_APPSUPPORT_H

#include <stdint.h>

typedef struct
{
    uint32_t app_id;
    const char *app_name;
    const char *bundle_id;
    uint64_t data_size;
    uint32_t support_level;
} library_appsupport_entry_t;

typedef struct
{
    uint32_t total_apps;
    uint32_t supported_apps;
    uint64_t total_support_data;
    uint32_t cache_hits;
} library_appsupport_state_t;

int library_appsupport_init(void);
int library_appsupport_register_app(library_appsupport_entry_t *entry);
int library_appsupport_unregister_app(uint32_t app_id);
int library_appsupport_get_app_data(uint32_t app_id, library_appsupport_entry_t *entry);
int library_appsupport_cleanup(uint32_t app_id);
int library_appsupport_get_state(library_appsupport_state_t *state);

#endif
