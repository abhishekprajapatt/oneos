#ifndef VAR_TMP_H
#define VAR_TMP_H

#include <stdint.h>

typedef struct
{
    uint32_t tmp_id;
    const char *file_name;
    const char *file_path;
    uint64_t file_size;
    uint64_t creation_time;
} var_tmp_entry_t;

typedef struct
{
    uint32_t total_temp_files;
    uint64_t total_temp_size;
    uint32_t expired_files;
} var_tmp_state_t;

int var_tmp_init(void);
int var_tmp_list(var_tmp_entry_t *files, uint32_t *count);
int var_tmp_create(const char *filename, const void *data, uint64_t size);
int var_tmp_get(const char *filename, var_tmp_entry_t *file);
int var_tmp_delete(const char *filename);
int var_tmp_cleanup_expired(void);
int var_tmp_get_state(var_tmp_state_t *state);

#endif
