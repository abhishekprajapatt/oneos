#ifndef LIBEXPAT_H
#define LIBEXPAT_H

#include <stdint.h>

typedef struct
{
    const char *tag_name;
    const char **attributes;
    uint32_t attr_count;
} libexpat_element_t;

int libexpat_init(void);
int libexpat_parse_buffer(const char *buffer, uint32_t size);
int libexpat_parse_file(const char *filename);
int libexpat_set_element_handler(void *handler);
int libexpat_set_character_handler(void *handler);
int libexpat_get_error(char *error_msg, uint32_t max_len);

#endif
