#ifndef SNAPSHOT_MANAGER_H
#define SNAPSHOT_MANAGER_H

#include <stdint.h>

typedef struct
{
    const char *snapshot_name;
    uint64_t creation_time;
    uint64_t snapshot_size;
    const char *description;
} snapshot_info_t;

int snapshot_manager_init(void);
int snapshot_manager_create_snapshot(const char *volume_path, const char *snapshot_name);
int snapshot_manager_delete_snapshot(const char *snapshot_name);
int snapshot_manager_restore_snapshot(const char *snapshot_name, const char *restore_path);
int snapshot_manager_list_snapshots(const char *volume_path, snapshot_info_t **snapshots, uint32_t *count);
int snapshot_manager_get_snapshot_info(const char *snapshot_name, snapshot_info_t *info);
int snapshot_manager_clone_snapshot(const char *snapshot_name, const char *clone_name);

#endif
