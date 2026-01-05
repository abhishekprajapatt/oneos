#ifndef HOME_PICTURES_H
#define HOME_PICTURES_H

#include <stdint.h>

typedef struct
{
    uint32_t image_id;
    const char *image_name;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint64_t file_size;
} home_pictures_image_t;

typedef struct
{
    uint32_t album_id;
    const char *album_name;
    uint32_t image_count;
    uint64_t total_size;
} home_pictures_album_t;

int home_pictures_init(const char *pictures_dir);
int home_pictures_create_album(const char *album_name);
int home_pictures_add_image_to_album(uint32_t album_id, const char *image_file);
int home_pictures_remove_image_from_album(uint32_t album_id, uint32_t image_id);
int home_pictures_view_image(uint32_t image_id);
int home_pictures_list_albums(home_pictures_album_t *albums, uint32_t max_count);

#endif
