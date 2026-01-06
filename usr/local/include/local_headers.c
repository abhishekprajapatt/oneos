#include "local_headers.h"

int local_headers_init(void)
{
    return 0;
}

int local_headers_register_header(const char *header_name, const char *header_path)
{
    if (!header_name || !header_path)
    {
        return -1;
    }
    return 0;
}

int local_headers_unregister_header(const char *header_name)
{
    if (!header_name)
    {
        return -1;
    }
    return 0;
}

int local_headers_list_headers(char **headers, uint32_t max_headers)
{
    if (!headers || max_headers == 0)
    {
        return -1;
    }
    return 0;
}

int local_headers_find_header(const char *header_name, char *path, uint32_t max_len)
{
    if (!header_name || !path || max_len == 0)
    {
        return -1;
    }
    return 0;
}

int local_headers_install_custom_header(const char *header_file)
{
    if (!header_file)
    {
        return -1;
    }
    return 0;
}
