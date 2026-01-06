#ifndef VOLUMES_H
#define VOLUMES_H

#include <stdint.h>

typedef struct
{
    const char *name;
    uint32_t total_size;
    uint32_t free_size;
} volume_info_t;

int volumes_init(void);
int volumes_list(volume_info_t **volumes, uint32_t *count);
int volumes_get_info(const char *volume_name, volume_info_t *info);
int volumes_format(const char *volume_name);
int volumes_mount(const char *volume_name, const char *mountpoint);

#endif
