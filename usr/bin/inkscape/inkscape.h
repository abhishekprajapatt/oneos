#ifndef INKSCAPE_H
#define INKSCAPE_H

#include <stdint.h>

typedef struct
{
    const char *input_file;
    const char *output_file;
    const char *export_format;
    uint32_t dpi;
    uint32_t width;
    uint32_t height;
} inkscape_export_options_t;

typedef struct
{
    uint32_t objects_count;
    uint32_t layers_count;
    const char *document_title;
    uint32_t width;
    uint32_t height;
} inkscape_document_info_t;

int inkscape_init(void);
int inkscape_open_document(const char *file_path);
int inkscape_export_to_png(const char *svg_file, const char *png_file);
int inkscape_export_with_options(inkscape_export_options_t *options);
int inkscape_get_document_info(const char *file, inkscape_document_info_t *info);
int inkscape_convert_to_path(const char *input_file, const char *output_file);

#endif
