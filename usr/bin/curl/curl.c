#include "curl.h"

int curl_init(void)
{
    return 0;
}

int curl_download(const char *url, const char *output_file)
{
    if (!url || !output_file)
    {
        return -1;
    }
    return 0;
}

int curl_upload(const char *url, const char *input_file)
{
    if (!url || !input_file)
    {
        return -1;
    }
    return 0;
}

int curl_request(curl_request_t *request, char *response_buffer, uint32_t buffer_size)
{
    if (!request || !request->url || !response_buffer || buffer_size == 0)
    {
        return -1;
    }
    return 0;
}

int curl_get_transfer_stats(curl_transfer_stats_t *stats)
{
    if (!stats)
    {
        return -1;
    }
    return 0;
}

int curl_cleanup(void)
{
    return 0;
}
