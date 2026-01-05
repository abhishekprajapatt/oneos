#ifndef MEDIA_IMAGE_H
#define MEDIA_IMAGE_H

#include <stdint.h>

typedef struct
{
    uint32_t image_id;
    const char *image_name;
    uint32_t width;
    uint32_t height;
    uint32_t format;
    uint64_t file_size;
} media_image_file_t;

typedef struct
{
    uint32_t image_count;
    uint64_t total_size;
    uint32_t loaded_images;
} media_image_state_t;

int media_image_init(void);
int media_image_load(const char *filename, media_image_file_t *image);
int media_image_unload(uint32_t image_id);
int media_image_scale(uint32_t image_id, uint32_t new_width, uint32_t new_height);
int media_image_rotate(uint32_t image_id, uint32_t degrees);
int media_image_save(uint32_t image_id, const char *filename);

#endif
