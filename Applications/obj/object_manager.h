#ifndef OBJECT_MANAGER_H
#define OBJECT_MANAGER_H

#include <stdint.h>

typedef struct
{
    uint32_t object_id;
    const char *object_type;
    const char *object_name;
    uint32_t reference_count;
} object_info_t;

typedef struct
{
    uint32_t total_objects;
    uint32_t active_objects;
    uint32_t inactive_objects;
    uint64_t total_memory;
} obj_stats_t;

int obj_manager_init(void);
int obj_manager_create(const char *type, const char *name, uint32_t *obj_id);
int obj_manager_destroy(uint32_t obj_id);
int obj_manager_get(uint32_t obj_id, object_info_t *info);
int obj_manager_list(object_info_t *objects, uint32_t *count);
int obj_manager_reference(uint32_t obj_id);
int obj_manager_dereference(uint32_t obj_id);
int obj_manager_get_stats(obj_stats_t *stats);

#endif
