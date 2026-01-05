#ifndef VAR_BACKUPS_H
#define VAR_BACKUPS_H

#include <stdint.h>

typedef struct
{
    uint32_t backup_id;
    const char *backup_name;
    const char *backup_path;
    uint64_t backup_size;
    uint64_t creation_time;
} var_backups_entry_t;

typedef struct
{
    uint32_t total_backups;
    uint32_t active_backups;
    uint64_t total_backup_size;
} var_backups_state_t;

int var_backups_init(void);
int var_backups_list(var_backups_entry_t *backups, uint32_t *count);
int var_backups_create(const char *name, const char *source_path);
int var_backups_restore(const char *backup_name, const char *destination);
int var_backups_delete(const char *backup_name);
int var_backups_get_state(var_backups_state_t *state);

#endif
