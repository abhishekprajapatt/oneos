#ifndef GIMP_H
#define GIMP_H

#include <stdint.h>

typedef struct
{
    uint32_t width;
    uint32_t height;
    const char *image_type;
    uint32_t color_depth;
} gimp_image_properties_t;

typedef struct
{
    const char *input_file;
    const char *output_file;
    const char *format;
    uint32_t quality;
    uint32_t compression;
} gimp_export_options_t;

int gimp_init(void);
int gimp_open_image(const char *file_path);
int gimp_create_new_image(gimp_image_properties_t *properties);
int gimp_apply_filter(const char *filter_name, void *filter_params);
int gimp_export_image(gimp_export_options_t *options);
int gimp_get_image_properties(const char *file, gimp_image_properties_t *props);

#endif
