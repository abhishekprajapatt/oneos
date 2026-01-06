#ifndef LIBCURL_H
#define LIBCURL_H

#include <stdint.h>

typedef struct
{
    const char *url;
    const char *method;
    void *body;
    uint32_t body_size;
} libcurl_request_t;

int libcurl_init(void);
int libcurl_perform_request(libcurl_request_t *request, void *response, uint32_t *response_size);
int libcurl_set_timeout(uint32_t timeout_ms);
int libcurl_set_header(const char *header_name, const char *header_value);
int libcurl_get_response_code(uint32_t *code);

#endif
