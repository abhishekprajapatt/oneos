#include "media_image.h"

int media_image_init(void)
{
    return 0;
}

int media_image_load(const char *filename, media_image_file_t *image)
{
    if (!filename || !image)
    {
        return -1;
    }
    return 0;
}

int media_image_unload(uint32_t image_id)
{
    return 0;
}

int media_image_scale(uint32_t image_id, uint32_t new_width, uint32_t new_height)
{
    return 0;
}

int media_image_rotate(uint32_t image_id, uint32_t degrees)
{
    return 0;
}

int media_image_save(uint32_t image_id, const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}
