#include "libexpat.h"

int libexpat_init(void)
{
    return 0;
}

int libexpat_parse_buffer(const char *buffer, uint32_t size)
{
    if (!buffer || size == 0)
    {
        return -1;
    }
    return 0;
}

int libexpat_parse_file(const char *filename)
{
    if (!filename)
    {
        return -1;
    }
    return 0;
}

int libexpat_set_element_handler(void *handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int libexpat_set_character_handler(void *handler)
{
    if (!handler)
    {
        return -1;
    }
    return 0;
}

int libexpat_get_error(char *error_msg, uint32_t max_len)
{
    if (!error_msg || max_len == 0)
    {
        return -1;
    }
    return 0;
}
