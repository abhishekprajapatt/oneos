#ifndef USR_SHARE_H
#define USR_SHARE_H

#include <stdint.h>

typedef struct
{
    uint32_t share_id;
    const char *resource_name;
    const char *resource_category;
    const char *resource_path;
    uint64_t size;
} usr_share_entry_t;

typedef struct
{
    uint32_t total_resources;
    uint32_t doc_files;
    uint32_t data_files;
    uint32_t locale_files;
    uint64_t total_size;
} usr_share_state_t;

int usr_share_init(void);
int usr_share_list_resources(usr_share_entry_t *resources, uint32_t *count);
int usr_share_get_resource(const char *name, usr_share_entry_t *resource);
int usr_share_search_by_category(const char *category, usr_share_entry_t *resources, uint32_t *count);
int usr_share_get_locales(char **locales, uint32_t *count);
int usr_share_get_state(usr_share_state_t *state);

#endif
