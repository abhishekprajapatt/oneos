#ifndef INCLUDE_UAPI_H
#define INCLUDE_UAPI_H

#include <stdint.h>

typedef struct
{
    uint32_t api_version;
    uint32_t feature_flags;
    uint32_t capability_mask;
} include_uapi_version_t;

typedef struct
{
    uint32_t ioctl_id;
    uint32_t direction;
    uint32_t data_size;
} include_uapi_ioctl_t;

int include_uapi_get_version(include_uapi_version_t *version);
int include_uapi_register_ioctl(include_uapi_ioctl_t *ioctl);
int include_uapi_unregister_ioctl(uint32_t ioctl_id);
int include_uapi_invoke_ioctl(uint32_t ioctl_id, void *data);
int include_uapi_get_feature(const char *feature_name);
int include_uapi_enable_feature(const char *feature_name);

#endif
