#ifndef HOME_DESKTOP_H
#define HOME_DESKTOP_H

#include <stdint.h>

typedef struct
{
    uint32_t file_id;
    const char *filename;
    uint32_t file_type;
    uint64_t file_size;
} home_desktop_file_t;

typedef struct
{
    uint32_t file_count;
    uint32_t total_size;
    uint32_t last_modified;
} home_desktop_state_t;

int home_desktop_init(const char *desktop_dir);
int home_desktop_list_files(home_desktop_file_t *files, uint32_t max_count);
int home_desktop_add_file(const char *filename);
int home_desktop_remove_file(const char *filename);
int home_desktop_get_file_info(const char *filename, home_desktop_file_t *file_info);
int home_desktop_open_file(const char *filename);

#endif
