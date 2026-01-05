#ifndef INCLUDE_LINUX_H
#define INCLUDE_LINUX_H

#include <stdint.h>

typedef struct
{
    uint32_t kernel_version;
    uint32_t api_version;
    uint32_t flags;
} include_linux_version_t;

typedef struct
{
    uint32_t subsystem_id;
    const char *subsystem_name;
    uint32_t status;
} include_linux_subsystem_t;

int include_linux_get_version(include_linux_version_t *version);
int include_linux_get_subsystem(const char *subsystem_name, include_linux_subsystem_t *subsystem);
int include_linux_register_subsystem(include_linux_subsystem_t *subsystem);
int include_linux_unregister_subsystem(uint32_t subsystem_id);
int include_linux_get_capability(const char *capability_name);
int include_linux_request_feature(const char *feature_name);

#endif
