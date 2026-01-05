#ifndef MEDIA_UTILITIES_H
#define MEDIA_UTILITIES_H

#include <stdint.h>

typedef struct
{
    const char *source_file;
    const char *destination_file;
    uint32_t source_format;
    uint32_t destination_format;
} media_utilities_conversion_t;

typedef struct
{
    uint32_t operation_count;
    uint32_t successful_operations;
    uint64_t total_bytes_processed;
} media_utilities_state_t;

int media_utilities_init(void);
int media_utilities_convert_format(media_utilities_conversion_t *conversion);
int media_utilities_compress(const char *input_file, const char *output_file);
int media_utilities_decompress(const char *input_file, const char *output_file);
int media_utilities_get_file_info(const char *filename, uint32_t *format, uint64_t *size);
int media_utilities_validate_file(const char *filename);

#endif
