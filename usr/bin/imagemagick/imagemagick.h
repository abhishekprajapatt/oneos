#ifndef IMAGEMAGICK_H
#define IMAGEMAGICK_H

#include <stdint.h>

typedef struct
{
    const char *input_file;
    const char *output_file;
    uint32_t width;
    uint32_t height;
    const char *filter;
} imagemagick_convert_options_t;

typedef struct
{
    uint32_t width;
    uint32_t height;
    const char *format;
    uint32_t color_depth;
    uint64_t file_size;
} imagemagick_image_info_t;

int imagemagick_init(void);
int imagemagick_convert(const char *input, const char *output);
int imagemagick_convert_with_options(imagemagick_convert_options_t *options);
int imagemagick_resize(const char *input, const char *output, uint32_t width, uint32_t height);
int imagemagick_get_image_info(const char *file, imagemagick_image_info_t *info);
int imagemagick_apply_effect(const char *input, const char *output, const char *effect_name);

#endif
