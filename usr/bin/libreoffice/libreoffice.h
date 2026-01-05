#ifndef LIBREOFFICE_H
#define LIBREOFFICE_H

#include <stdint.h>

typedef struct
{
    const char *document_path;
    const char *output_format;
    const char *output_path;
    uint32_t headless_mode;
    uint32_t convert_only;
} libreoffice_convert_options_t;

typedef struct
{
    uint32_t total_documents;
    uint32_t successfully_converted;
    uint32_t failed_conversions;
    uint64_t total_processing_time;
} libreoffice_stats_t;

int libreoffice_init(void);
int libreoffice_open_document(const char *document_path);
int libreoffice_convert_document(const char *input, const char *output, const char *format);
int libreoffice_convert_with_options(libreoffice_convert_options_t *options);
int libreoffice_get_document_info(const char *document, char *info, uint32_t info_size);
int libreoffice_get_stats(libreoffice_stats_t *stats);

#endif
