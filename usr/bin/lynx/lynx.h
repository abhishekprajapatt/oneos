#ifndef LYNX_H
#define LYNX_H

#include <stdint.h>

typedef struct
{
    const char *url;
    const char *start_file;
    uint32_t use_cookies;
    uint32_t display_images;
    const char *user_agent;
} lynx_session_options_t;

typedef struct
{
    const char *page_title;
    uint32_t total_links;
    uint32_t total_forms;
    uint64_t page_size;
} lynx_page_info_t;

int lynx_init(void);
int lynx_browse_url(const char *url);
int lynx_open_file(const char *file_path);
int lynx_start_session(lynx_session_options_t *options);
int lynx_get_page_info(lynx_page_info_t *info);
int lynx_extract_text(const char *url, char *text_buffer, uint32_t buffer_size);

#endif
