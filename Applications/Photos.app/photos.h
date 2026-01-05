#ifndef PHOTOS_H
#define PHOTOS_H

#include <stdint.h>

typedef struct
{
    uint32_t photo_id;
    const char *photo_name;
    const char *photo_path;
    uint32_t width;
    uint32_t height;
    uint32_t created_time;
} photo_info_t;

typedef struct
{
    uint32_t total_photos;
    uint32_t total_albums;
    uint64_t total_size;
} photos_stats_t;

int photos_init(void);
int photos_import_photo(const char *file_path);
int photos_create_album(const char *album_name);
int photos_add_to_album(uint32_t photo_id, const char *album_name);
int photos_remove_from_album(uint32_t photo_id, const char *album_name);
int photos_delete_photo(uint32_t photo_id);
int photos_edit_photo(uint32_t photo_id, const char *edit_type);
int photos_search_photos(const char *query, photo_info_t *results, uint32_t *count);
int photos_get_stats(photos_stats_t *stats);

#endif
