#include "libcurl.h"

int libcurl_init(void)
{
    return 0;
}

int libcurl_perform_request(libcurl_request_t *request, void *response, uint32_t *response_size)
{
    if (!request || !response || !response_size)
    {
        return -1;
    }
    return 0;
}

int libcurl_set_timeout(uint32_t timeout_ms)
{
    if (timeout_ms == 0)
    {
        return -1;
    }
    return 0;
}

int libcurl_set_header(const char *header_name, const char *header_value)
{
    if (!header_name || !header_value)
    {
        return -1;
    }
    return 0;
}

int libcurl_get_response_code(uint32_t *code)
{
    if (!code)
    {
        return -1;
    }
    return 0;
}
