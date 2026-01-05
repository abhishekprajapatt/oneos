#ifndef FIREFOX_H
#define FIREFOX_H

#include <stdint.h>

typedef struct
{
    const char *profile_name;
    const char *url;
    uint32_t private_mode;
    uint32_t incognito;
    const char *proxy_server;
} firefox_launch_options_t;

typedef struct
{
    uint32_t tabs_open;
    uint32_t windows_open;
    uint64_t memory_usage;
    uint32_t active_downloads;
} firefox_session_info_t;

int firefox_init(void);
int firefox_launch(void);
int firefox_launch_with_url(const char *url);
int firefox_launch_with_options(firefox_launch_options_t *options);
int firefox_get_session_info(firefox_session_info_t *info);
int firefox_close_window(uint32_t window_id);

#endif
