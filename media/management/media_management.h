#ifndef MEDIA_MANAGEMENT_H
#define MEDIA_MANAGEMENT_H

#include <stdint.h>

typedef struct
{
    uint32_t media_id;
    const char *media_name;
    uint32_t media_type;
    uint64_t capacity;
    uint64_t used_space;
} media_management_device_t;

typedef struct
{
    uint32_t device_count;
    uint64_t total_capacity;
    uint64_t total_used;
} media_management_state_t;

int media_management_init(void);
int media_management_register_device(media_management_device_t *device);
int media_management_unregister_device(uint32_t media_id);
int media_management_format_device(uint32_t media_id);
int media_management_get_device_info(uint32_t media_id, media_management_device_t *info);
int media_management_eject_device(uint32_t media_id);

#endif
