#include "firefox.h"

int firefox_init(void)
{
    return 0;
}

int firefox_launch(void)
{
    return 0;
}

int firefox_launch_with_url(const char *url)
{
    if (!url)
    {
        return -1;
    }
    return 0;
}

int firefox_launch_with_options(firefox_launch_options_t *options)
{
    if (!options)
    {
        return -1;
    }
    return 0;
}

int firefox_get_session_info(firefox_session_info_t *info)
{
    if (!info)
    {
        return -1;
    }
    return 0;
}

int firefox_close_window(uint32_t window_id)
{
    return 0;
}
