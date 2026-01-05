#include "home_pictures.h"

int home_pictures_init(const char *pictures_dir)
{
    if (!pictures_dir)
    {
        return -1;
    }
    return 0;
}

int home_pictures_create_album(const char *album_name)
{
    if (!album_name)
    {
        return -1;
    }
    return 0;
}

int home_pictures_add_image_to_album(uint32_t album_id, const char *image_file)
{
    if (!image_file)
    {
        return -1;
    }
    return 0;
}

int home_pictures_remove_image_from_album(uint32_t album_id, uint32_t image_id)
{
    return 0;
}

int home_pictures_view_image(uint32_t image_id)
{
    return 0;
}

int home_pictures_list_albums(home_pictures_album_t *albums, uint32_t max_count)
{
    if (!albums)
    {
        return -1;
    }
    return 0;
}
