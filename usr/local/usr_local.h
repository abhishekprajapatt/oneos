#ifndef USR_LOCAL_H
#define USR_LOCAL_H

#include <stdint.h>

typedef struct
{
    uint32_t local_id;
    const char *resource_name;
    const char *resource_type;
    const char *resource_path;
    uint64_t size;
} usr_local_entry_t;

typedef struct
{
    uint32_t total_resources;
    uint32_t binaries;
    uint32_t libraries;
    uint32_t configurations;
    uint64_t total_size;
} usr_local_state_t;

int usr_local_init(void);
int usr_local_list_resources(usr_local_entry_t *resources, uint32_t *count);
int usr_local_get_resource(const char *name, usr_local_entry_t *resource);
int usr_local_add_resource(const char *name, const char *type, const char *path);
int usr_local_remove_resource(const char *name);
int usr_local_get_state(usr_local_state_t *state);

#endif
