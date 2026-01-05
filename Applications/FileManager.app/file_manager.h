#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdint.h>

typedef struct
{
    uint32_t file_id;
    const char *file_name;
    const char *file_path;
    uint64_t file_size;
    uint32_t is_directory;
    uint32_t created_time;
    uint32_t modified_time;
} file_info_t;

typedef struct
{
    uint32_t total_files;
    uint32_t total_directories;
    uint64_t total_size;
    uint32_t files_selected;
} file_manager_stats_t;

int file_manager_init(void);
int file_manager_list_directory(const char *path, file_info_t *files, uint32_t *count);
int file_manager_get_file_info(const char *file_path, file_info_t *info);
int file_manager_create_directory(const char *path);
int file_manager_delete_file(const char *file_path);
int file_manager_rename_file(const char *old_path, const char *new_name);
int file_manager_copy_file(const char *source, const char *destination);
int file_manager_move_file(const char *source, const char *destination);
int file_manager_get_stats(file_manager_stats_t *stats);

#endif
