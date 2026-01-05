#ifndef MOUNT_UTILITIES_H
#define MOUNT_UTILITIES_H

#include <stdint.h>

typedef struct
{
    uint32_t mount_id;
    const char *device_name;
    const char *mount_point;
} mount_utilities_config_t;

int mount_utilities_init(void);
int mount_utilities_mount_filesystem(const char *device, const char *mount_point);
int mount_utilities_unmount_filesystem(const char *mount_point);
int mount_utilities_list_mounts(void);
int mount_utilities_get_mount_info(const char *mount_point);
int mount_utilities_check_filesystem(const char *device);

#endif
