#ifndef MOUNT_H
#define MOUNT_H

#include <stdint.h>

int mount_init(void);
int mount_device(const char *device, const char *mountpoint, const char *fstype);
int mount_unmount(const char *mountpoint);
int mount_list(char **mounts, uint32_t max_mounts);
int mount_get_info(const char *mountpoint, void **info);

#endif
