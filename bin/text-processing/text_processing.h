#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H

#include <stdint.h>

typedef struct
{
    uint32_t processor_id;
    const char *processor_name;
    const char *version;
} text_processing_config_t;

int text_processing_init(void);
int text_processing_search_text(const char *file_path, const char *pattern);
int text_processing_replace_text(const char *file_path, const char *pattern, const char *replacement);
int text_processing_count_lines(const char *file_path);
int text_processing_sort_file(const char *file_path);
int text_processing_concatenate_files(const char **files, uint32_t count);
int text_processing_extract_columns(const char *file_path, uint32_t column);

#endif
