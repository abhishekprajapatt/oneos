#ifndef FILE_UTILITIES_H
#define FILE_UTILITIES_H

#include <stdint.h>

typedef struct
{
    uint32_t util_id;
    const char *util_name;
    const char *version;
} file_utilities_config_t;

int file_utilities_init(void);
int file_utilities_copy_file(const char *source, const char *destination);
int file_utilities_move_file(const char *source, const char *destination);
int file_utilities_delete_file(const char *file_path);
int file_utilities_list_directory(const char *directory);
int file_utilities_get_file_info(const char *file_path);
int file_utilities_change_permissions(const char *file_path, uint32_t permissions);

#endif
