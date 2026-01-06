#include "iso_builder.h"

int iso_builder_init(void)
{
    return 0;
}

int iso_builder_add_file(const char *file_path, const char *iso_path)
{
    if (!file_path || !iso_path)
    {
        return -1;
    }
    return 0;
}

int iso_builder_add_directory(const char *dir_path, const char *iso_path)
{
    if (!dir_path || !iso_path)
    {
        return -1;
    }
    return 0;
}

int iso_builder_create(const char *iso_output_path)
{
    if (!iso_output_path)
    {
        return -1;
    }
    return 0;
}

int iso_builder_set_boot_image(const char *boot_image_path)
{
    if (!boot_image_path)
    {
        return -1;
    }
    return 0;
}

int iso_builder_verify_image(const char *iso_path)
{
    if (!iso_path)
    {
        return -1;
    }
    return 0;
}
