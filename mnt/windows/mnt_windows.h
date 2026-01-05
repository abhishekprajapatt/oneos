#ifndef MNT_WINDOWS_H
#define MNT_WINDOWS_H

#include <stdint.h>

typedef struct
{
    uint32_t win_mount_id;
    const char *drive_letter;
    const char *network_path;
    uint32_t mount_type;
    uint32_t status;
} mnt_windows_mount_t;

typedef struct
{
    uint32_t total_windows_mounts;
    uint32_t active_windows_mounts;
    uint32_t network_shares;
    uint32_t mapped_drives;
} mnt_windows_state_t;

int mnt_windows_init(void);
int mnt_windows_mount_drive(mnt_windows_mount_t *mount);
int mnt_windows_unmount_drive(uint32_t win_mount_id);
int mnt_windows_map_network_share(const char *path, const char *drive_letter);
int mnt_windows_unmap_network_share(const char *drive_letter);
int mnt_windows_get_state(mnt_windows_state_t *state);

#endif
