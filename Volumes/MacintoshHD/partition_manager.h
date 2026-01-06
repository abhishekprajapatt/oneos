#ifndef PARTITION_MANAGER_H
#define PARTITION_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *partition_name;
    uint64_t start_sector;
    uint64_t end_sector;
    const char *partition_type;
    uint32_t flags;
} partition_info_t;

int partition_manager_init(void);
int partition_manager_create_partition(const char *device, partition_info_t *partition);
int partition_manager_delete_partition(const char *partition_path);
int partition_manager_resize_partition(const char *partition_path, uint64_t new_size);
int partition_manager_list_partitions(const char *device, partition_info_t **partitions, uint32_t *count);
int partition_manager_get_partition_info(const char *partition_path, partition_info_t *info);
int partition_manager_set_boot_flag(const char *partition_path);

#endif
