#ifndef CURL_H
#define CURL_H

#include <stdint.h>

typedef struct
{
    const char *url;
    const char *method;
    const char *user_agent;
    uint32_t timeout;
    uint32_t follow_redirects;
} curl_request_t;

typedef struct
{
    uint32_t response_code;
    uint64_t bytes_downloaded;
    uint64_t bytes_uploaded;
    uint64_t transfer_time;
} curl_transfer_stats_t;

int curl_init(void);
int curl_download(const char *url, const char *output_file);
int curl_upload(const char *url, const char *input_file);
int curl_request(curl_request_t *request, char *response_buffer, uint32_t buffer_size);
int curl_get_transfer_stats(curl_transfer_stats_t *stats);
int curl_cleanup(void);

#endif
