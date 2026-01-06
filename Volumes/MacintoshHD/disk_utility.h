#ifndef DISK_UTILITY_H
#define DISK_UTILITY_H

#include <stdint.h>

int disk_utility_init(void);
int disk_utility_repair_disk(const char *volume_path);
int disk_utility_verify_disk(const char *volume_path);
int disk_utility_defragment(const char *volume_path);
int disk_utility_create_image(const char *image_path, uint64_t size);
int disk_utility_restore_image(const char *image_path, const char *device);
int disk_utility_erase_volume(const char *volume_path, const char *new_name);

#endif
